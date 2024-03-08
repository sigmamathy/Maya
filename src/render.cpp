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

void RenderResource::CleanUp()
{
	rc.resources.erase(this);
}

RenderContext::RenderContext()
	: window(0), crntvao(0), crntprogram(0)
{
	resources.reserve(32);
}

void RenderContext::CleanAll()
{
	Begin();
	while (!resources.empty()) {
		auto it = resources.begin();
		(*it)->CleanUp();
	}
}

void RenderContext::Begin()
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->resptr));
}

void RenderContext::ClearBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderContext::DrawSetup()
{
	MAYA_DIF(!crntvao || !crntprogram)
	{
		Error::Send(Error::MissingReference,
			"RenderContext::DrawSetup(): The vertex array or shader program is absent.");
		return;
	}

	if (crntvao->iboid)
	{
		Ivec2 dr = crntvao->draw_range.x != -1 ? crntvao->draw_range * 3 : Ivec2 { 0, crntvao->indices_count };
		glDrawElements(GL_TRIANGLES, dr.y - dr.x, GL_UNSIGNED_INT, (void*)(size_t)dr.x);
	}
	else
	{
		Ivec2 dr = crntvao->draw_range.x != -1 ? crntvao->draw_range : Ivec2 { 0, crntvao->vertex_count };
		glDrawArrays(GL_TRIANGLES, dr.x, dr.y - dr.x);
	}

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

}