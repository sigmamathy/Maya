#include <maya/renderer.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MAYA_TEXTURE_SLOTS_COUNT 16

extern unsigned Maya_s_binded_vao;
extern unsigned Maya_s_binded_shader_program;

template<class Ty, class... Tys>
static bool s_Equals(Ty value, Tys... args)
{
	return ((value == args) && ...);
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

	Program->window->UseGraphicsContext();

	if (Maya_s_binded_vao != Input->vaoid) {
		glBindVertexArray(Input->vaoid);
		Maya_s_binded_vao = Input->vaoid;
	}

	if (Maya_s_binded_shader_program != Program->programid) {
		glUseProgram(Program->programid);
		Maya_s_binded_shader_program = Program->programid;
	}

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