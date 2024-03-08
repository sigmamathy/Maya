#include <maya/window.hpp>
#include <maya/vertexarray.hpp>
#include <maya/shader.hpp>
#include <maya/dataio.hpp>
#include <maya/texture.hpp>

static float v[] = {
	-0.5f, -0.5f,	0, 0,
	0.5f, -0.5f,	1, 0,
	0.0f, 0.5f,		0.5f, 1,
};

static char const* vshader = R"(

#version 330 core

layout(location = 0) in vec2 ipos;
layout(location = 1) in vec2 itexcoord;

out vec2 vtexcoord;

void main() {
	gl_Position = vec4(ipos, 0, 1);
	vtexcoord = itexcoord;
}

)";

static char const* fshader = R"(

#version 330 core

in vec2 vtexcoord;
out vec4 FragColor;

uniform sampler2D tex;

void main() {
	FragColor = texture(tex, vtexcoord);
}

)";

int __cdecl main(int argc, char** argv)
{
	maya::LibraryManager m;
	m.LoadDependency(maya::GraphicsDep);
	maya::Error::SetGlobalCallback(maya::Error::LogToConsole);

	maya::Window::uptr window = maya::Window::MakeUnique();
	auto& rc = window->GetRenderContext();
	rc.Begin();

	maya::VertexLayout layout;
	layout.Push(0, 2).Push(1, 2);

	maya::VertexArray::uptr vao = maya::VertexArray::MakeUnique(rc);
	vao->PushBuffer(maya::ConstBuffer(v, sizeof(v)), layout);

	maya::ShaderProgram::uptr program = maya::ShaderProgram::MakeUnique(rc);
	program->CompileShader(maya::VertexShader, vshader);
	program->CompileShader(maya::FragmentShader, fshader);
	program->LinkProgram();

	maya::ImageImporter image(MAYA_PROJECT_SOURCE_DIR "/tests/image0.png");

	maya::Texture::uptr texture = maya::Texture::MakeUnique(rc);
	texture->CreateContent(image.Data, image.Size, image.Channels);
	texture->SetFilterLinear();
	texture->SetRepeat();

	while (!window->IsRequestedForClose())
	{
		rc.ClearBuffer();

		rc.SetInput(vao.get());
		rc.SetProgram(program.get());
		rc.SetTexture(texture.get(), 0);

		program->SetUniform<int>("tex", 0);

		rc.DrawSetup();

		window->SwapBuffers();
		maya::Window::PollEvents();
	}

	window.reset();


	return 0;
}