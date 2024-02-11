#include <maya/renderer.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MAYA_TEXTURE_SLOTS_COUNT 16

struct s_CrntBindCache
{
	unsigned VAO, Program;
	unsigned Test;
};

static MayaHashMap<MayaWindow*, s_CrntBindCache> s_bind_cache;

static void s_InitCacheIfNotPresent(MayaWindow* window)
{
	if (s_bind_cache.count(window))
		return;
	auto& x = s_bind_cache[window];
	x.VAO = 0;
	x.Program = 0;
	x.Test = MayaNoTest;
}

void Maya_s_BindVertexArray(MayaWindow* window, unsigned vaoid)
{
	s_InitCacheIfNotPresent(window);
	if (s_bind_cache.at(window).VAO != vaoid) {
		window->UseGraphicsContext();
		glBindVertexArray(vaoid);
		s_bind_cache.at(window).VAO = vaoid;
	}
}

void Maya_s_BindShaderProgram(MayaWindow* window, unsigned programid)
{
	s_InitCacheIfNotPresent(window);
	if (s_bind_cache.at(window).Program != programid) {
		window->UseGraphicsContext();
		glUseProgram(programid);
		s_bind_cache.at(window).Program = programid;
	}
}

static void s_SetEnableTest(MayaWindow* window, MayaPerFragTest test, unsigned gltest, bool enable)
{
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

void MayaSetScissorRect(MayaWindow* window, MayaFvec2 pos, MayaFvec2 size)
{
	s_InitCacheIfNotPresent(window);
	s_SetEnableTest(window, MayaScissorTest, GL_SCISSOR_TEST, true);
	glScissor((int)pos.x, window->GetSize().y - (int)pos.y - (int)size.y, (int)size.x, (int)size.y);
}

void MayaRenderer::ExecuteDraw()
{
	MAYA_DIF(!Input) {
		MAYA_SERR(MAYA_EMPTY_REFERENCE_ERROR, "MayaRenderer::ExecuteDraw(): Input is nullptr");
		return;
	}

	MAYA_DIF(!Program) {
		MAYA_SERR(MAYA_EMPTY_REFERENCE_ERROR, "MayaRenderer::ExecuteDraw(): Program is nullptr");
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
		MAYA_SERR(MAYA_INVALID_GRAPHICS_CONTEXT_ERROR,
			"MayaRenderer::ExecuteDraw(): Resources used in the process refers to different graphics context.");
		return;
	}
#endif

	auto* window = Program->window;
	window->UseGraphicsContext();

	Maya_s_BindVertexArray(window, Input->vaoid);
	Maya_s_BindShaderProgram(window, Program->programid);

	s_SetEnableTest(window, MayaScissorTest, GL_SCISSOR_TEST, (bool)(Test & MayaScissorTest));
	s_SetEnableTest(window, MayaBlending, GL_BLEND, (bool)(Test & MayaBlending));

	for (int i = 0; i < MAYA_TEXTURE_SLOTS_COUNT; i++)
	{
		if (Textures[i] != 0) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, Textures[i]->textureid);
		}
	}

	if (Input->iboid)
		glDrawElements(GL_TRIANGLES, Input->indices_draw_count, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, Input->vertex_count);
}