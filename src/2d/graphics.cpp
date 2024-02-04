#include <maya/2d/graphics.hpp>
#include <maya/2d/camera.hpp>
#include <maya/2d/textdisplay.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>

static MayaString s_vertex_shader = R"(

#version 330 core

layout(location = 0) in vec2 iPos;
layout(location = 1) in vec2 iTexCoord;

out vec2 vTexCoord;

uniform mat4 uProjection, uView, uModel;

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(iPos, 0, 1);
	vTexCoord = iTexCoord;
}

)";

static MayaString s_fragment_shader = R"(

#version 330 core

in vec2 vTexCoord;

out vec4 oFragColor;

uniform vec4 uColor;

const int TEXTURE_SLOTS = 2;
uniform int uHasTexture[TEXTURE_SLOTS];
uniform sampler2D uTexture[TEXTURE_SLOTS];

void main()
{
	oFragColor = uColor;
	for (int i = 0; i < TEXTURE_SLOTS; i++) {
		if (uHasTexture[i] == 1)
			oFragColor = oFragColor * texture(uTexture[i], vTexCoord);
	}
}

)";

static MayaVertexArrayUptr s_CreateCircleVertexArray(MayaWindow& window, int precision)
{
	MayaArrayList<float> vertices;
	vertices.reserve(precision * 4);

	for (int i = 0; i < precision; i++)
	{
		float radian = static_cast<float>(2 * MayaConstPI * i / precision);
		float c = std::cosf(radian), s = std::sinf(radian);
		vertices.emplace_back(c * 0.5f);
		vertices.emplace_back(s * 0.5f);
		vertices.emplace_back(c * 0.5f + 0.5f);
		vertices.emplace_back(s * 0.5f + 0.5f);
	}

	MayaArrayList<unsigned> indices;
	indices.reserve(3 * precision - 6);

	for (int i = 1; i <= precision - 2; i++)
	{
		indices.emplace_back(0);
		indices.emplace_back(i);
		indices.emplace_back(i + 1);
	}

	MayaVertexArrayUptr result = MayaCreateVertexArrayUptr(window);
	result->SetVertexCount(precision);
	MayaVertexLayout layout;
	layout (0, 2) (1, 2);
	layout.Data = vertices.data();
	layout.Size = static_cast<unsigned>(vertices.size() * sizeof(float));
	result->LinkVertexBuffer(layout);
	result->LinkIndexBuffer(indices.data(), static_cast<unsigned>(indices.size() * sizeof(unsigned)));
	return result;
}

MayaGraphics2D::MayaGraphics2D(MayaWindow* window)
	: Window(window), texture(nullptr), camera(nullptr)
{
	MayaShaderProgramParameters param;
	param.Vertex = s_vertex_shader;
	param.Fragment = s_fragment_shader;
	program = MayaCreateShaderProgramUptr(*window, param);
	program->SetUniformMatrix("uModel", MayaFmat4(1));
	program->SetUniformMatrix("uView", MayaFmat4(1));
	program->SetUniformMatrix("uProjection", MayaFmat4(1));
	program->SetUniformVector("uColor", MayaWhite / 255.0f);

	program->SetUniform<int>("uHasTexture[0]", 0);
	program->SetUniform<int>("uHasTexture[1]", 0);
	program->SetUniform<int>("uTexture[0]", 0);
	program->SetUniform<int>("uTexture[1]", 1);

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

	circlevao64 = s_CreateCircleVertexArray(*window, 64);
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

void MayaGraphics2D::UseCamera(Camera* camera)
{
	this->camera = camera;
	program->SetUniformMatrix("uView", camera ? camera->GetViewMatrix() : MayaFmat4(1));
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
	program->SetUniform<int>("uHasTexture[0]", texture ? 1 : 0);
}

void MayaGraphics2D::DrawRect(float x, float y, float width, float height)
{
	DrawRect(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2D::DrawRect(MayaFvec2 pos, MayaFvec2 size)
{
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	program->SetUniformMatrix("uModel", MayaTranslate(pos) * MayaScale(size));
	MayaRenderer r;
	r.Input = squarevao.get();
	r.Program = program.get();
	r.Textures[0] = texture;
	r.ExecuteDraw();
}

void MayaGraphics2D::DrawOval(float x, float y, float width, float height)
{
	DrawOval(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2D::DrawOval(MayaFvec2 pos, MayaFvec2 size)
{
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	program->SetUniformMatrix("uModel", MayaTranslate(pos) * MayaScale(size));
	MayaRenderer r;
	r.Input = circlevao64.get();
	r.Program = program.get();
	r.Textures[0] = texture;
	r.ExecuteDraw();
}

void MayaGraphics2D::DrawText(MayaFont& font, MayaStringCR text, float x, float y)
{
	return DrawText(font, text, MayaFvec2(x, y));
}

void MayaGraphics2D::DrawText(MayaFont& font, MayaStringCR text, MayaFvec2 pos)
{
	program->SetUniform<int>("uHasTexture[1]", 1);
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	int advance = 0;
	for (char c : text)
	{
		auto& glyph = font[c];
		MayaFvec2 tpos = { 
			advance + glyph.Bearing.x + glyph.Size.x * 0.5f,
			glyph.Bearing.y - glyph.Size.y + glyph.Size.y * 0.5f
		};
		program->SetUniformMatrix("uModel", MayaTranslate(pos + tpos) * MayaScale(glyph.Size));
		MayaRenderer r;
		r.Input = squarevao.get();
		r.Program = program.get();
		r.Textures[0] = texture;
		r.Textures[1] = glyph.Texture;
		r.ExecuteDraw();
		advance += glyph.Advance;
	}
	program->SetUniform<int>("uHasTexture[1]", 0);
}

void MayaGraphics2D::DrawText(TextDisplay& text)
{
	text.ComputeGlobalModel();
	program->SetUniform<int>("uHasTexture[1]", 1);
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	for (int i = 0; i < text.char_models.size(); i++)
	{
		program->SetUniformMatrix("uModel", text.global_model * text.char_models[i]);
		MayaRenderer r;
		r.Input = squarevao.get();
		r.Program = program.get();
		r.Textures[0] = texture;
		r.Textures[1] = (*text.font)[text.string[i]].Texture;
		r.ExecuteDraw();
	}
	program->SetUniform<int>("uHasTexture[1]", 0);
}