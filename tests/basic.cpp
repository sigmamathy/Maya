#include <maya/window.hpp>
#include <maya/vertexarray.hpp>
#include <maya/shader.hpp>
#include <maya/dataio.hpp>
#include <maya/texture.hpp>
#include <maya/transformation.hpp>
#include <maya/audio.hpp>

static float v[] = {
	-0.5f, -0.5f,	0, 0,
	0.5f, -0.5f,	1, 0,
	-0.5f, 0.5f,	0, 1,
	0.5f, -0.5f,	1, 0,
	-0.5f, 0.5f,	0, 1,
	0.5f, 0.5f,		1, 1,
};

static char const* vshader = R"(

#version 330 core

layout(location = 0) in vec2 ipos;
layout(location = 1) in vec2 itexcoord;

out vec2 vtexcoord;

uniform mat4 uModel, uProj;

void main() {
	gl_Position = uProj * uModel * vec4(ipos, 0, 1);
	vtexcoord = itexcoord;
}

)";

static char const* fshader = R"(

#version 330 core

in vec2 vtexcoord;
out vec4 oFragColor;

uniform sampler2D uTex;

void main() {
	oFragColor = texture(uTex, vtexcoord);
}

)";

int __cdecl main(int argc, char** argv)
{
	maya::LibraryManager m;
	m << maya::GraphicsDep << maya::AudioDep;

	maya::Error::SetGlobalHandle(maya::Error::LogToConsole);

	maya::Window::uptr window = maya::Window::MakeUnique();
	auto& rc = window->GetRenderContext();
	rc.Begin();

	rc.Enable(rc.Blending);
	rc.SetBlendMode(rc.AlphaBlend);

	maya::VertexLayout layout;
	layout.Push(0, 2).Push(1, 2);

	maya::VertexArray::uptr vao = maya::VertexArray::MakeUnique(rc);
	vao->PushBuffer(maya::ConstBuffer(v, sizeof(v)), layout);

	maya::ShaderProgram::uptr program = maya::ShaderProgram::MakeUnique(rc);
	program->CompileShader(maya::VertexShader, vshader);
	program->CompileShader(maya::FragmentShader, fshader);
	program->LinkProgram();

	program->SetUniformMatrix("uProj", maya::OrthogonalProjection(window->GetSize()));
	program->SetUniform<int>("uTex", 0);
	
	maya::FontData font;
	font.ImportFile(MAYA_PROJECT_SOURCE_DIR "/tests/Arial.ttf", 50, rc);

	maya::AudioData audio;
	audio.ImportFile(MAYA_PROJECT_SOURCE_DIR "/tests/Dash.mp3");

	maya::AudioPlayer player;
	player.SetSource(&audio);
	player.Start();

	while (!window->IsRequestedForClose())
	{
		rc.ClearBuffer();

		rc.SetInput(vao.get());
		rc.SetProgram(program.get());

		maya::stl::string text = std::to_string(player.GetPosition()) + " / " + std::to_string(player.GetDuration());
		int adv = 0;
		for (char c : text) {
			auto& glyph = font.Data.at(c);
			rc.SetTexture(glyph.Texture.get(), 0);
			program->SetUniformMatrix("uModel",
				maya::TranslateModel(glyph.Bearing + maya::Fvec2(adv, 0) + maya::Fvec2(glyph.Size.x, -glyph.Size.y) / 2)
				* maya::ScaleModel(glyph.Size));
			rc.DrawSetup();
			adv += glyph.Advance;
		}

		// ui::Manager m (window);
		// 

		window->SwapBuffers();
		maya::Window::PollEvents();
	}

	window.reset();
	player.Stop();

	return 0;
}