#include <maya/2d/graphics.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>

static MayaString s_vertex_shader = R"(

#version 330 core

layout(location = 0) in vec2 iPos;
layout(location = 1) in vec2 iTexCoord;

out vec2 vTexCoord;

uniform mat4 uProjection, uModel;

void main()
{
	gl_Position = uProjection * uModel * vec4(iPos, 0, 1);
	vTexCoord = iTexCoord;
}

)";

static MayaString s_fragment_shader = R"(

#version 330 core

in vec2 vTexCoord;

out vec4 oFragColor;

uniform vec4 uColor;
uniform bool uHasTexture;
uniform sampler2D uTexture;

void main()
{
	oFragColor = uColor;
	if (uHasTexture)
		oFragColor = oFragColor * texture(uTexture, vTexCoord);
}

)";

MayaGraphics2D::MayaGraphics2D(MayaWindow* window)
	: Window(window)
{
	MayaShaderProgramParameters param;
	param.Vertex = s_vertex_shader;
	param.Fragment = s_fragment_shader;
	program = MayaCreateShaderProgramUptr(*window, param);
	program->SetUniformMatrix("uModel", MayaFmat4(1));
	program->SetUniformMatrix("uProjection", MayaFmat4(1));
	program->SetUniformVector("uColor", MayaWhite);
	program->SetUniform<bool>("uHasTexture", false);
	program->SetUniform<int>("uTexture", 0);

	static float square_vertices[] = {
		-0.5f, 0.5f,		0, 1,
		-0.5f, -0.5f,		0, 0,
		0.5f, 0.5f,			1, 1,
		-0.5f, -0.5f,		0, 0,
		0.5f, 0.5f,			1, 1,
		0.5f, -0.5f,		1, 0
	};

	squarevao = MayaCreateVertexArrayUptr(*window);
	squarevao->SetVertexCount(6);
	MayaVertexLayout layout;
	layout (0, 2) (1, 2);
	layout.Data = square_vertices;
	layout.Size = sizeof(square_vertices);
	squarevao->LinkVertexBuffer(layout);
}

void MayaGraphics2D::UseProjection(float width, float height)
{
	UseProjection(MayaFvec2(width, height));
}

void MayaGraphics2D::UseProjection(MayaFvec2 size)
{
	MayaFmat4 proj = MayaOrthogonalProjection(size);
	program->SetUniformMatrix("uProjection", proj);
}

void MayaGraphics2D::UseWindowProjection()
{
	UseProjection(Window->GetSize());
}

void MayaGraphics2D::UseColor(int r, int g, int b, int a)
{
	UseColor(MayaIvec4(r, g, b, a));
}

void MayaGraphics2D::UseColor(MayaIvec3 color)
{
	UseColor(MayaIvec4(color.x, color.y, color.z, 255));
}

void MayaGraphics2D::UseColor(MayaIvec4 color)
{
	MayaFvec4 col = color / 255.0f;
	program->SetUniformVector("uColor", col);
}

void MayaGraphics2D::UseColor(unsigned hexcode, bool hasopacity)
{
	MayaIvec4 color = MayaConvertHexToColor(hexcode, hasopacity);
	UseColor(color);
}

void MayaGraphics2D::UseTexture(MayaTexture* texture)
{
	this->texture = texture;
	program->SetUniform<bool>("uHasTexture", texture);
}

void MayaGraphics2D::DrawRect(float x, float y, float width, float height)
{
	DrawRect(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2D::DrawRect(MayaFvec2 pos, MayaFvec2 size)
{
	program->SetUniformMatrix("uModel", MayaTranslate(pos) * MayaScale(size));
	MayaRenderer r;
	r.Input = squarevao.get();
	r.Program = program.get();
	r.Textures[0] = texture;
	r.ExecuteDraw();
}