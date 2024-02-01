#include <maya/renderer.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern unsigned Maya_s_binded_vao;
extern unsigned Maya_s_binded_shader_program;

void MayaRenderer::ExecuteDraw()
{
	MAYA_DIF(Input->window != Program->window)
		MAYA_SERR(MAYA_INVALID_GRAPHICS_CONTEXT_ERROR,
			"MayaRenderer::ExecuteDraw(): Resources used in the process refers to different graphics context.");

	Program->window->UseGraphicsContext();

	if (Maya_s_binded_vao != Input->vaoid) {
		glBindVertexArray(Input->vaoid);
		Maya_s_binded_vao = Input->vaoid;
	}

	if (Maya_s_binded_shader_program != Program->programid) {
		glUseProgram(Program->programid);
		Maya_s_binded_shader_program = Program->programid;
	}

	glDrawArrays(GL_TRIANGLES, 0, Input->vertex_count);
}