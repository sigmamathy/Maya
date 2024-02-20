#include <maya/2d/graphics.hpp>
#include <maya/2d/camera.hpp>
#include <maya/2d/textdisplay.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>
#include <glad/glad.h>

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

static MayaShaderProgramUptr s_CreateShaderProgram(MayaWindow& window)
{
	MayaShaderProgramParameters param;
	param.Vertex = s_vertex_shader;
	param.Fragment = s_fragment_shader;

	MayaShaderProgramUptr program = MayaCreateShaderProgramUptr(window, param);

	program->SetUniformMatrix("uModel", MayaFmat4(1));
	program->SetUniformMatrix("uView", MayaFmat4(1));
	program->SetUniformMatrix("uProjection", MayaFmat4(1));
	program->SetUniformVector("uColor", MayaWhite / 255.0f);

	program->SetUniform<int>("uHasTexture[0]", 0);
	program->SetUniform<int>("uHasTexture[1]", 0);
	program->SetUniform<int>("uTexture[0]", 0);
	program->SetUniform<int>("uTexture[1]", 1);

	return program;
}

#define MAYA_SQUARE			0
#define MAYA_CIRCLE			1
#define MAYA_TICK			2
#define MAYA_ISO_TRIANGLE	3

static MayaVertexArrayUptr s_CreateSquareVertexArray(MayaWindow& window)
{
	static constexpr float vertices[] = {
		-0.5f, 0.5f,		0, 1,
		-0.5f, -0.5f,		0, 0,
		0.5f, 0.5f,			1, 1,
		-0.5f, -0.5f,		0, 0,
		0.5f, 0.5f,			1, 1,
		0.5f, -0.5f,		1, 0
	};

	MayaVertexArrayUptr res = MayaCreateVertexArrayUptr(window);
	MayaVertexLayout layout;
	layout(0, 2) (1, 2);
	res->LinkVertexBuffer(MayaBuffer(vertices, sizeof(vertices)), layout, false);

	return res;
}

static MayaVertexArrayUptr s_CreateCircleVertexArray(MayaWindow& window, int precision)
{
	MayaArrayList<float> vertices;
	vertices.reserve(precision * 4 + 4);

	vertices.emplace_back(0.0f);
	vertices.emplace_back(0.0f);
	vertices.emplace_back(0.5f);
	vertices.emplace_back(0.5f);

	for (int i = 0; i < precision; i++)
	{
		float radian = static_cast<float>(2 * MayaConstPI * i / precision);
		float c = cosf(radian), s = sinf(radian);
		vertices.emplace_back(c * 0.5f);
		vertices.emplace_back(s * 0.5f);
		vertices.emplace_back(c * 0.5f + 0.5f);
		vertices.emplace_back(s * 0.5f + 0.5f);
	}

	MayaArrayList<unsigned> indices;
	indices.reserve(3 * precision);

	for (int i = 1; i <= precision - 1; i++)
	{
		indices.emplace_back(0);
		indices.emplace_back(i);
		indices.emplace_back(i + 1);
	}

	indices.emplace_back(0);
	indices.emplace_back(precision);
	indices.emplace_back(1);

	MayaVertexArrayUptr result = MayaCreateVertexArrayUptr(window);
	MayaVertexLayout layout;
	layout (0, 2) (1, 2);
	result->LinkVertexBuffer(MayaBuffer(vertices.data(), unsigned(vertices.size() * sizeof(float))), layout);
	result->LinkIndexBuffer(MayaBuffer(indices.data(), unsigned(indices.size() * sizeof(unsigned))));
	return result;
}

static MayaVertexArrayUptr s_CreateTickVertexArray(MayaWindow& window)
{
	static constexpr float vertices[] = {
		-0.5f, -0.05f,				0, 0.45f,
		-0.37f, 0.0f,				0.13f, 0.5f,
		-0.267f, -0.481f,			0.234f, 0.02f,
		-0.225f, -0.252f,			0.275f, 0.248f,
		-0.222f, -0.5f,				0.278f, 0.0f,
		-0.163f, -0.47f,			0.337f, 0.03f,
		0.245f, 0.5f,				0.745f, 1.0f,
		0.352f, 0.461f,				0.825f, 0.961f,
		0.5f, 0.439f,				1.0f, 0.939f
	};

	static constexpr unsigned indices[] = {
		0, 1, 2,
		1, 2, 3,
		2, 3, 4,
		3, 4, 5,
		3, 5, 6,
		5, 6, 7,
		5, 7, 8
	};

	MayaVertexArrayUptr res = MayaCreateVertexArrayUptr(window);

	MayaVertexLayout layout;
	layout(0, 2) (1, 2);

	res->LinkVertexBuffer(MayaBuffer(vertices, sizeof(vertices)), layout);
	res->LinkIndexBuffer(MayaBuffer(indices, sizeof(indices)));

	return res;
}

static MayaVertexArrayUptr s_CreateIsoTriangleVertexArray(MayaWindow& window)
{
	static constexpr float vertices[] = {
		-0.5f, -0.5f,		0, 0,
		0.5f, -0.5f,		1, 0,
		0.0f, 0.5f,			0.5f, 1,
	};

	MayaVertexArrayUptr res = MayaCreateVertexArrayUptr(window);
	MayaVertexLayout layout;
	layout(0, 2) (1, 2);

	res->LinkVertexBuffer(MayaBuffer(vertices, sizeof(vertices)), layout);
	return res;
}

MayaGraphics2d::MayaGraphics2d(MayaWindow& window)
	: Window(&window), texture(nullptr), camera(nullptr), projection(1), color(MayaWhite), has_scissor(false)
{
	program = s_CreateShaderProgram(window);
	vao[MAYA_SQUARE]		= s_CreateSquareVertexArray(window);
	vao[MAYA_CIRCLE]		= s_CreateCircleVertexArray(window, 100);
	vao[MAYA_TICK]			= s_CreateTickVertexArray(window);
	vao[MAYA_ISO_TRIANGLE]	= s_CreateIsoTriangleVertexArray(window);
}

void MayaGraphics2d::UseProjection(float width, float height)
{
	UseProjection(MayaFvec2(width, height));
}

void MayaGraphics2d::UseProjection(MayaFvec2 size)
{
	if (projection == size)
		return;
	MayaFmat4 proj = MayaOrthogonalProjection(size);
	program->SetUniformMatrix("uProjection", proj);
	projection = size;
}

void MayaGraphics2d::UseWindowProjection()
{
	UseProjection(Window->GetSize());
}

void MayaGraphics2d::UseCamera(MayaCamera2d* camera)
{
	if (this->camera == camera)
		return;
	this->camera = camera;
	program->SetUniformMatrix("uView", camera ? camera->GetViewMatrix() : MayaFmat4(1));
}

void MayaGraphics2d::UseColor(int r, int g, int b, int a)
{
	UseColor(MayaIvec4(r, g, b, a));
}

void MayaGraphics2d::UseColor(MayaIvec3 color)
{
	UseColor(MayaIvec4(color.x, color.y, color.z, 255));
}

void MayaGraphics2d::UseColor(MayaIvec4 color)
{
	if (this->color == color)
		return;
	MayaFvec4 col = color / 255.0f;
	program->SetUniformVector("uColor", col);
	this->color = color;
}

void MayaGraphics2d::UseColor(unsigned hexcode, bool hasopacity)
{
	MayaIvec4 color = MayaConvertHexToColor(hexcode, hasopacity);
	UseColor(color);
}

void MayaGraphics2d::UseTexture(MayaTexture* texture)
{
	if (this->texture == texture)
		return;
	this->texture = texture;
	program->SetUniform<int>("uHasTexture[0]", texture ? 1 : 0);
}

void MayaGraphics2d::UseScissor(bool use)
{
	has_scissor = use;
}

void MayaGraphics2d::DrawShape(int index)
{
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	MayaRenderer r;
	r.Input = vao[index].get();
	r.Program = program.get();
	r.Textures[0] = texture;
	r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
	r.Test |= MayaBlending;
	r.ExecuteDraw();
}

void MayaGraphics2d::DrawShape(int index, MayaFvec2 pos, MayaFvec2 size)
{
	program->SetUniformMatrix("uModel", MayaTranslate(pos) * MayaScale(size));
	DrawShape(index);
}

void MayaGraphics2d::DrawRect(float x, float y, float width, float height)
{
	DrawRect(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2d::DrawRect(MayaFvec2 pos, MayaFvec2 size)
{
	DrawShape(MAYA_SQUARE, pos, size);
}

void MayaGraphics2d::DrawRectBorder(float x, float y, float width, float height, int linewidth)
{
	DrawRectBorder(MayaFvec2(x, y), MayaFvec2(width, height), linewidth);
}

void MayaGraphics2d::DrawRectBorder(MayaFvec2 pos, MayaFvec2 size, int linewidth)
{
	program->SetUniform<int>("uHasTexture[0]", 0);
	DrawShape(MAYA_SQUARE, MayaFvec2(pos.x - (size.x - linewidth) * 0.5f, pos.y), MayaFvec2(linewidth, size.y));
	DrawShape(MAYA_SQUARE, MayaFvec2(pos.x + (size.x - linewidth) * 0.5f, pos.y), MayaFvec2(linewidth, size.y));
	DrawShape(MAYA_SQUARE, MayaFvec2(pos.x, pos.y - (size.y - linewidth) * 0.5f), MayaFvec2(size.x, linewidth));
	DrawShape(MAYA_SQUARE, MayaFvec2(pos.x, pos.y + (size.y - linewidth) * 0.5f), MayaFvec2(size.x, linewidth));
	program->SetUniform<int>("uHasTexture[0]", texture ? 1 : 0);
}

void MayaGraphics2d::DrawLine(float startx, float starty, float endx, float endy)
{
	DrawLine(MayaFvec2(startx, starty), MayaFvec2(endx, endy));
}

void MayaGraphics2d::DrawLine(MayaFvec2 start, MayaFvec2 end)
{
	if (start == end) return;
	MayaFmat4 pos = MayaTranslate((start + end) / 2.0f);
	MayaFvec2 dv = end - start;
	MayaFmat4 scale = MayaScale(MayaFvec2(dv.Norm(), 1));
	MayaFmat4 rot = MayaRotate(std::atan2(dv[1], dv[0]));
	program->SetUniformMatrix("uModel", pos * rot * scale);
	program->SetUniform<int>("uHasTexture[0]", 0);
	DrawShape(MAYA_SQUARE);
	program->SetUniform<int>("uHasTexture[0]", texture ? 1 : 0);
}

void MayaGraphics2d::DrawOval(float x, float y, float width, float height, int completion)
{
	DrawOval(MayaFvec2(x, y), MayaFvec2(width, height), completion);
}

void MayaGraphics2d::DrawOval(MayaFvec2 pos, MayaFvec2 size, int completion)
{
	vao[MAYA_CIRCLE]->SetDrawRange(0, completion);
	DrawShape(MAYA_CIRCLE, pos, size);
}

void MayaGraphics2d::DrawIsoTriangle(float x, float y, float width, float height)
{
	DrawIsoTriangle(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2d::DrawIsoTriangle(MayaFvec2 pos, MayaFvec2 size)
{
	DrawShape(MAYA_ISO_TRIANGLE, pos, size);
}

void MayaGraphics2d::DrawText(MayaFont& font, MayaStringCR text, float x, float y, MayaCorner align)
{
	return DrawText(font, text, MayaFvec2(x, y), align);
}

void MayaGraphics2d::DrawText(MayaFont& font, MayaStringCR text, MayaFvec2 pos, MayaCorner align)
{
	program->SetUniform<int>("uHasTexture[1]", 1);
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	MayaFvec2 tsize(0);

	if (align != MayaCornerBL) {
		for (int i = 0, advance = 0; i < text.size(); i++)
		{
			auto& glyph = font[text[i]];
			tsize.x += glyph.Advance;
			if (glyph.Bearing.y > tsize.y)
				tsize.y = static_cast<float>(glyph.Bearing[1]);
			advance += glyph.Advance;
		}
	}

	int advance = 0;
	for (char c : text)
	{
		auto& glyph = font[c];
		MayaFvec2 tpos = { 
			advance + glyph.Bearing.x + glyph.Size.x * 0.5f,
			glyph.Bearing.y - glyph.Size.y + glyph.Size.y * 0.5f
		};

		MayaFvec2 d;
		d.x = -tsize.x * ((align & 0b11) - 1) * 0.5f;
		d.y = -tsize.y * ((align >> 2) - 1) * 0.5f;

		program->SetUniformMatrix("uModel", MayaTranslate(pos + tpos + d) * MayaScale(glyph.Size));
		MayaRenderer r;
		r.Input = vao[MAYA_SQUARE].get();
		r.Program = program.get();
		r.Textures[0] = texture;
		r.Textures[1] = glyph.Texture;
		r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
		r.Test |= MayaBlending;
		r.ExecuteDraw();
		advance += glyph.Advance;
	}
	program->SetUniform<int>("uHasTexture[1]", 0);
}

void MayaGraphics2d::DrawText(MayaTextDisplay2d& text)
{
	DrawText(text, 0, text.GetLength());
}

void MayaGraphics2d::DrawText(MayaTextDisplay2d& text, int start, int end)
{
	text.ComputeGlobalModel();
	program->SetUniform<int>("uHasTexture[1]", 1);
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	for (int i = start; i < end; i++)
	{
		program->SetUniformMatrix("uModel", text.global_model * text.char_models[i]);
		MayaRenderer r;
		r.Input = vao[MAYA_SQUARE].get();
		r.Program = program.get();
		r.Textures[0] = texture;
		r.Textures[1] = (*text.font)[text.string[i]].Texture;
		r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
		r.Test |= MayaBlending;
		r.ExecuteDraw();
	}
	program->SetUniform<int>("uHasTexture[1]", 0);
}

void MayaGraphics2d::DrawTick(float x, float y, float width, float height)
{
	DrawTick(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2d::DrawTick(MayaFvec2 pos, MayaFvec2 size)
{
	DrawShape(MAYA_TICK, pos, size);
}