#include <maya/render.hpp>
#include <maya/window.hpp>
#include <maya/vertexarray.hpp>
#include <maya/shader.hpp>
#include <maya/texture.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace maya
{

RenderResource::RenderResource(void)
	: rc(0), nativeid(0)
{
}

void RenderResource::Init(class RenderContext& rc)
{
	MAYA_ASSERT(!this->rc && !nativeid);
	this->rc = &rc;
	rc.BeginContext();
	rc.resources.insert(this);
}

void RenderResource::Free()
{
	MAYA_ASSERT(rc);
	rc->BeginContext();
	rc->resources.erase(this);
	rc = 0;
}

static RenderContext* s_current_context = 0;

void RenderContext::Init(Window* win)
{
	window			= win;
	input			= 0;
	program			= 0;
	settings		= 0;
	blendmode		= NO_BLEND;
	num_wait		= 0;
	num_quiet_wait	= 0;
	threadid		= std::this_thread::get_id();

	resources.reserve(32);
	GLint num_tex_slots;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num_tex_slots);
	textures.resize(num_tex_slots);
}

void RenderContext::Free()
{
	BeginContext();
	while (!resources.empty()) {
		auto it = resources.begin();
		(*it)->Free();
	}
}

void RenderContext::BeginContext()
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetNativePointer()));
	s_current_context = this;
}

RenderContext* RenderContext::CurrentContext()
{
	return s_current_context;
}

void RenderContext::ClearBuffer()
{
	Disable(SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderContext::SetInput(VertexArray* resource)
{
	if (input == resource) return;
	input = resource;
	glBindVertexArray(resource ? resource->GetNativeId() : 0);
}

void RenderContext::SetProgram(ShaderProgram* pg)
{
	if (program == pg) return;
	program = pg;
	glUseProgram(program ? program->GetNativeId() : 0);
}

void RenderContext::SetTexture(Texture* tex, int slot)
{
	if (textures[slot] == tex) return;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex ? tex->GetNativeId() : 0);
	textures[slot] = tex;
}

void RenderContext::Enable(Options set)
{
	if (IsEnabled(set)) return;
	GLenum e = 0;
	switch (set) {
		case BLENDING: e = GL_BLEND; break;
		case SCISSOR_TEST: e = GL_SCISSOR_TEST; break;
	}
	glEnable(e);
	settings &= 1 << set;
}

void RenderContext::Disable(Options set)
{
	if (!IsEnabled(set)) return;
	GLenum e = 0;
	switch (set) {
		case BLENDING: e = GL_BLEND; break;
		case SCISSOR_TEST: e = GL_SCISSOR_TEST; break;
	}
	glDisable(e);
	settings &= ~(1 << set);
}

void RenderContext::SetBlendMode(BlendMode bm)
{
	if (blendmode == bm)
		return;
	switch (bm) {
		case NO_BLEND: glBlendFunc(GL_ONE, GL_ZERO); break;
		case ALPHA_BLEND: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case ADDITIVE_BLEND: glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
		case MULTIPLICATIVE_BLEND: glBlendFunc(GL_DST_COLOR, GL_ZERO); break;
	}
	blendmode = bm;
}

void RenderContext::DrawSetup()
{
#if MAYA_DEBUG
	if (!input || !program)
	{
		auto& cm = *CoreManager::Instance();
		cm.MakeError(cm.INVALID_OPERATION_ERROR,
			"Attempting to draw without vertex array or shader program presented.");
		return;
	}
#endif

	if (input->HasIndexBuffer()) {
		Ivec2 dr = input->GetDrawRange() * 3;
		glDrawElements(GL_TRIANGLES, dr.y - dr.x, GL_UNSIGNED_INT, (void*)(size_t)dr.x);
	}
	else {
		Ivec2 dr = input->GetDrawRange();
		glDrawArrays(GL_TRIANGLES, dr.x, dr.y - dr.x);
	}
}

RenderContext::QuietWait::QuietWait(RenderContext& rc) : rc(rc)
{
	rc.num_quiet_wait++;
}

RenderContext::QuietWait::~QuietWait()
{
	rc.num_quiet_wait--;
}

void RenderContext::WaitForSyncExec(stl::fnptr<void()> const& exec)
{
	if (std::this_thread::get_id() == threadid) {
		exec();
		return;
	}
	std::unique_lock<std::mutex> lock(mut);
	num_wait++;
	cv.wait(lock);
	execsync = exec;
	num_wait--;
	cv0.notify_one();
	cv1.wait(lock);
}

void RenderContext::SyncWithThreads(float maxwait)
{
	auto* cm = CoreManager::Instance();
	float start = cm->GetTimeSince();

	for (;;)
	{
		if (!num_wait)
		{
			if (!num_quiet_wait)
				return; // no one is waiting.
			
			while (!num_wait) // loop until wait call.
			{
				if (!num_quiet_wait || cm->GetTimeSince() - start > maxwait) {
					return; // wait time exceeds or no threads requires wait anymore.
				}
			}

			continue; // someone is waiting.
		}

		if (cm->GetTimeSince() - start > maxwait)
			return; // wait time exceeds.

		std::unique_lock<std::mutex> lock(mut);
		cv.notify_one(); // unlock any wait.
		cv0.wait(lock); // wait here.
		execsync(); // execute whatever recieved.
		cv1.notify_one(); // tell thread to continue execution.
	}
}

}