#include <maya/renderer.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stack>

#define MAYA_TEXTURE_SLOTS_COUNT 16

struct s_CrntBindCache
{
	unsigned VAO		= 0;
	unsigned Program	= 0;
	unsigned Test		= MayaNoTest;
	std::stack<MayaIvec4> ScissorRects;
};

static MayaHashMap<MayaWindow*, s_CrntBindCache> s_bind_cache;

static void s_InitCacheIfNotPresent(MayaWindow* window)
{
	if (s_bind_cache.count(window))
		return;
	auto& x = s_bind_cache[window];
}

void Maya_s_BindVertexArray(MayaWindow* window, unsigned vaoid)
{
	if (s_bind_cache.at(window).VAO != vaoid) {
		window->UseGraphicsContext();
		glBindVertexArray(vaoid);
		s_bind_cache.at(window).VAO = vaoid;
	}
}

void Maya_s_BindShaderProgram(MayaWindow* window, unsigned programid)
{
	if (s_bind_cache.at(window).Program != programid) {
		window->UseGraphicsContext();
		glUseProgram(programid);
		s_bind_cache.at(window).Program = programid;
	}
}

void Maya_s_SetEnableTest(MayaWindow* window, unsigned test, unsigned gltest, bool enable)
{
	s_InitCacheIfNotPresent(window);

	if (enable)
	{
		if (!(s_bind_cache.at(window).Test & test)) {
			glEnable(gltest);
			s_bind_cache.at(window).Test |= test;
		}
	}
	else
	{
		if (s_bind_cache.at(window).Test & test) {
			glDisable(gltest);
			s_bind_cache.at(window).Test &= ~test;
		}
	}
}

void MayaPushScissorRect(MayaWindow* window, MayaIvec2 pos, MayaIvec2 size)
{
	Maya_s_SetEnableTest(window, MayaScissorTest, GL_SCISSOR_TEST, true);
	auto& sr = s_bind_cache.at(window).ScissorRects;

	if (!sr.empty())
	{
		auto p2 = pos + size;
		auto prev = sr.top();
		if (prev.x > pos.x) pos.x = prev.x;
		if (prev.y > pos.y) pos.y = prev.y;
		if (prev.x + prev.z < p2.x) p2.x = prev.x + prev.z;
		if (prev.y + prev.w < p2.y) p2.y = prev.y + prev.w;
		size = p2 - pos;
	}

	sr.push(MayaConcat(pos, size));
	glScissor(pos.x, window->GetSize().y - pos.y - size.y, size.x, size.y);
}

void MayaPopScissorRect(MayaWindow* window)
{
	s_InitCacheIfNotPresent(window);
	auto& sr = s_bind_cache.at(window).ScissorRects;
	if (sr.empty())
		return;
	sr.pop();
	if (sr.empty())
		return;
	auto s = sr.top();
	glScissor(s.x, window->GetSize().y - s.y - s.w, s.z, s.w);
}

void MayaRenderer::ExecuteDraw()
{
	MAYA_DIF(!Input) {
		MayaSendError({ MAYA_EMPTY_REFERENCE_ERROR, "MayaRenderer::ExecuteDraw(): Input is nullptr" });
		return;
	}

	MAYA_DIF(!Program) {
		MayaSendError({ MAYA_EMPTY_REFERENCE_ERROR, "MayaRenderer::ExecuteDraw(): Program is nullptr" });
		return;
	}

#if MAYA_DEBUG
	bool alleq = Input->window == Program->window;
	for (int i = 0; i < MAYA_TEXTURE_SLOTS_COUNT; i++)
	{
		if (Textures[i] != 0 && Textures[i]->window != Input->window) {
			alleq = false;
			break;
		}
	}
	if (!alleq) {
		MayaSendError({ MAYA_INCONSISTENTENCY_ERROR,
			"MayaRenderer::ExecuteDraw(): Graphics context used is inconsistence." });
		return;
	}
#endif

	auto* window = Program->window;
	window->UseGraphicsContext();

	Maya_s_BindVertexArray(window, Input->vaoid);
	Maya_s_BindShaderProgram(window, Program->programid);

	Maya_s_SetEnableTest(window, MayaScissorTest, GL_SCISSOR_TEST, Test & MayaScissorTest);
	Maya_s_SetEnableTest(window, MayaBlending, GL_BLEND, Test & MayaBlending);

	for (int i = 0; i < MAYA_TEXTURE_SLOTS_COUNT; i++)
	{
		if (Textures[i] != 0) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, Textures[i]->textureid);
		}
	}

	if (Input->iboid)
	{
		MayaIvec2 dr = Input->draw_range.x != -1 ? Input->draw_range * 3 : MayaIvec2{0, Input->indices_count};
		glDrawElements(GL_TRIANGLES, dr.y - dr.x, GL_UNSIGNED_INT, (void*)(size_t)dr.x);
	}
	else
	{
		MayaIvec2 dr = Input->draw_range.x != -1 ? Input->draw_range : MayaIvec2{ 0, Input->vertex_count };
		glDrawArrays(GL_TRIANGLES, dr.x, dr.y - dr.x);
	}
}