#include <maya/render.hpp>
#include <maya/window.hpp>
#include <maya/vertexarray.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace maya
{

RenderResource::RenderResource(RenderContext& rc)
	: rc(rc)
{
	rc.resources.insert(this);
}

void RenderResource::Destroy()
{
	rc.resources.erase(this);
}

static RenderContext* s_current_context = 0;

RenderContext::RenderContext()
	: window(0), input(0), program(0), settings(0), blendmode(NoBlend),
	num_wait(0), num_quiet_wait(0), threadid(std::this_thread::get_id())
{
}

void RenderContext::Init()
{
	GLint texmax;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texmax);
	textures.resize(texmax);
	resources.reserve(32);
}

void RenderContext::Begin()
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
	Disable(ScissorTest);
	glClear(GL_COLOR_BUFFER_BIT);
}

VertexArray* RenderContext::GetInput()
{
	return input;
}

ShaderProgram* RenderContext::GetProgram()
{
	return program;
}

Texture* RenderContext::GetTexture(int slot)
{
	return textures[slot];
}

void RenderContext::Enable(Setting set)
{
	if (IsEnabled(set)) return;
	GLenum e = 0;
	switch (set) {
		case Blending: e = GL_BLEND; break;
		case ScissorTest: e = GL_SCISSOR_TEST; break;
	}
	glEnable(e);
	settings &= 1 << set;
}

void RenderContext::Disable(Setting set)
{
	if (!IsEnabled(set)) return;
	GLenum e = 0;
	switch (set) {
		case Blending: e = GL_BLEND; break;
		case ScissorTest: e = GL_SCISSOR_TEST; break;
	}
	glDisable(e);
	settings &= ~(1 << set);
}

bool RenderContext::IsEnabled(Setting set) const
{
	return settings & (1 << set);
}

void RenderContext::SetBlendMode(BlendMode bm)
{
	if (blendmode == bm)
		return;
	switch (bm) {
		case NoBlend: glBlendFunc(GL_ONE, GL_ZERO); break;
		case AlphaBlend: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case AdditiveBlend: glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
		case MulitplicativeBlend: glBlendFunc(GL_DST_COLOR, GL_ZERO); break;
	}
	blendmode = bm;
}

RenderContext::BlendMode RenderContext::GetBlendMode()
{
	return blendmode;
}

void RenderContext::DrawSetup()
{
	MAYA_DIF(!input || !program)
	{
		Error::Send(Error::MissingReference,
			"RenderContext::DrawSetup(): The vertex array or shader program is absent.");
		return;
	}

	if (input->HasIndexBuffer()) {
		Ivec2 dr = input->GetDrawRange() * 3;
		glDrawElements(GL_TRIANGLES, dr.y - dr.x, GL_UNSIGNED_INT, (void*)(size_t)dr.x);
	}
	else {
		Ivec2 dr = input->GetDrawRange();
		glDrawArrays(GL_TRIANGLES, dr.x, dr.y - dr.x);
	}
}

int RenderContext::GetMaxTextureSlots() const
{
	return static_cast<unsigned>(textures.size());
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

void RenderContext::DestroyAll()
{
	Begin();
	while (!resources.empty()) {
		auto it = resources.begin();
		(*it)->Destroy();
	}
}

}