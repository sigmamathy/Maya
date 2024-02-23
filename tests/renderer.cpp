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
	MayaBlending* Blending = 0;
	MayaScissorTest* Scissor = 0;
};

static MayaHashMap<MayaWindow*, s_CrntBindCache> s_bind_cache;

void Maya_s_InitCache(MayaWindow* window)
{
	s_bind_cache[window];
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

void Maya_s_BindBlending(MayaWindow* window, MayaBlending* test)
{
	auto& b = s_bind_cache.at(window).Blending;
	if (b != test)
	{
		window->UseGraphicsContext();
		if (test) {
			if (!b)
				glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, test->Func);
		}
		else if (!test && b)
			glDisable(GL_BLEND);
		b = test;
	}
}

void Maya_s_BindScissorTest(MayaWindow* window, MayaScissorTest* test)
{
	auto& s = s_bind_cache.at(window).Scissor;
	if (s != test)
	{
		window->UseGraphicsContext();
		if (test) {
			if (!s)
				glEnable(GL_SCISSOR_TEST);
			glScissor(test->Position.x, window->GetSize().y - test->Position.y - test->Size.y,
				test->Size.x, test->Size.y);
		}
		else if (!test && s)
			glDisable(GL_SCISSOR_TEST);
		s = test;
	}
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

	Maya_s_BindBlending(window, Blending);
	Maya_s_BindScissorTest(window, Scissor);

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