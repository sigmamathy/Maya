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

static MayaShaderProgramUptr s_CreateShaderProgram(Window& window)
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

static MayaVertexArrayUptr s_CreateSquareVertexArray(Window& window)
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

static MayaVertexArrayUptr s_CreateCircleVertexArray(Window& window, int precision)
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

static MayaVertexArrayUptr s_CreateTickVertexArray(Window& window)
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

static MayaVertexArrayUptr s_CreateIsoTriangleVertexArray(Window& window)
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

MayaGraphics2d::MayaGraphics2d(Window& window)
	: Window(&window), texture(nullptr), camera(nullptr), rotation(0), projection(1), color(MayaWhite)
{
	program = s_CreateShaderProgram(window);
	vao[MAYA_SQUARE]		= s_CreateSquareVertexArray(window);
	vao[MAYA_CIRCLE]		= s_CreateCircleVertexArray(window, 100);
	vao[MAYA_TICK]			= s_CreateTickVertexArray(window);
	vao[MAYA_ISO_TRIANGLE]	= s_CreateIsoTriangleVertexArray(window);
	blend.SrcFactor = blend.SrcAlpha;
	blend.DestFactor = blend.OneMinusSrcAlpha;

	r.Program = program.get();
	r.Textures[0] = texture;
	r.MaxNumTexture = 2;
	r.Blending = &blend;
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

void MayaGraphics2d::UseRotation(float rot)
{
	rotation = rot;
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

void MayaGraphics2d::PushScissor(MayaFvec2 pos, MayaFvec2 size)
{
	MayaFvec2 wsz = Window->GetSize();
	float xs = wsz.x / projection.x, ys = wsz.y / projection.y;
	pos.x *= xs;
	pos.y *= ys;
	size.x *= xs;
	size.y *= ys;
	pos = pos + wsz * 0.5f - size * 0.5f;
	pos.y = wsz.y - pos.y - size.y;

	if (!scissors.empty())
	{
		auto p2 = pos + size;
		auto prev = scissors.back();
		if (prev.Position.x > pos.x) pos.x = (float) prev.Position.x;
		if (prev.Position.y > pos.y) pos.y = (float) prev.Position.y;
		if (prev.Position.x + prev.Size.x < p2.x) p2.x = (float) prev.Position.x + prev.Size.x;
		if (prev.Position.y + prev.Size.y < p2.y) p2.y = (float) prev.Position.y + prev.Size.y;
		size = p2 - pos;
	}

	scissors.emplace_back(pos, size);
	r.ScissorTest = &scissors.back();
}

void MayaGraphics2d::PopScissor()
{
	scissors.pop_back();
	r.ScissorTest = scissors.empty() ? 0 : &scissors.back();
}

void MayaGraphics2d::UpdateCameraOnDraw()
{
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
}

void MayaGraphics2d::SetModel(MayaFvec2 pos, MayaFvec2 size)
{
	program->SetUniformMatrix("uModel", MayaTranslate(pos) * MayaScale(size));
}

void MayaGraphics2d::SetModelRotate(MayaFvec2 pos, MayaFvec2 size)
{
	program->SetUniformMatrix("uModel", MayaTranslate(pos) * MayaRotate(rotation) * MayaScale(size));
}

void MayaGraphics2d::DrawRect(float x, float y, float width, float height)
{
	DrawRect(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2d::DrawRect(MayaFvec2 pos, MayaFvec2 size)
{
	UpdateCameraOnDraw();
	SetModelRotate(pos, size);
	r.Input = vao[MAYA_SQUARE].get();
	r.ExecuteDraw();
}

void MayaGraphics2d::DrawRectBorder(float x, float y, float width, float height, int linewidth)
{
	DrawRectBorder(MayaFvec2(x, y), MayaFvec2(width, height), linewidth);
}

void MayaGraphics2d::DrawRectBorder(MayaFvec2 pos, MayaFvec2 size, int linewidth)
{
	UpdateCameraOnDraw();
	r.Input = vao[MAYA_SQUARE].get();
	program->SetUniform<int>("uHasTexture[0]", 0);

	SetModelRotate(MayaFvec2(pos.x - (size.x - linewidth) * 0.5f, pos.y), MayaFvec2(linewidth, size.y));
	r.ExecuteDraw(); // left
	SetModelRotate(MayaFvec2(pos.x + (size.x - linewidth) * 0.5f, pos.y), MayaFvec2(linewidth, size.y));
	r.ExecuteDraw(); // right
	SetModelRotate(MayaFvec2(pos.x, pos.y - (size.y - linewidth) * 0.5f), MayaFvec2(size.x, linewidth));
	r.ExecuteDraw(); // bottom
	SetModelRotate(MayaFvec2(pos.x, pos.y + (size.y - linewidth) * 0.5f), MayaFvec2(size.x, linewidth));
	r.ExecuteDraw(); // top

	program->SetUniform<int>("uHasTexture[0]", texture ? 1 : 0);
}

void MayaGraphics2d::DrawLine(float startx, float starty, float endx, float endy)
{
	DrawLine(MayaFvec2(startx, starty), MayaFvec2(endx, endy));
}

void MayaGraphics2d::DrawLine(MayaFvec2 start, MayaFvec2 end)
{
	if (start == end) return;
	UpdateCameraOnDraw();
	r.Input = vao[MAYA_SQUARE].get();
	MayaFmat4 pos = MayaTranslate((start + end) / 2.0f);
	MayaFvec2 dv = end - start;
	MayaFmat4 scale = MayaScale(MayaFvec2(dv.Norm(), 1));
	MayaFmat4 rot = MayaRotate(std::atan2(dv[1], dv[0]));
	program->SetUniformMatrix("uModel", pos * rot * scale);
	program->SetUniform<int>("uHasTexture[0]", 0);
	r.ExecuteDraw();
	program->SetUniform<int>("uHasTexture[0]", texture ? 1 : 0);
}

void MayaGraphics2d::DrawOval(float x, float y, float width, float height, int completion)
{
	DrawOval(MayaFvec2(x, y), MayaFvec2(width, height), completion);
}

void MayaGraphics2d::DrawOval(MayaFvec2 pos, MayaFvec2 size, int completion)
{
	UpdateCameraOnDraw();
	r.Input = vao[MAYA_CIRCLE].get();
	vao[MAYA_CIRCLE]->SetDrawRange(0, completion);
	SetModelRotate(pos, size);
	r.ExecuteDraw();
}

void MayaGraphics2d::DrawIsoTriangle(float x, float y, float width, float height)
{
	DrawIsoTriangle(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2d::DrawIsoTriangle(MayaFvec2 pos, MayaFvec2 size)
{
	UpdateCameraOnDraw();
	r.Input = vao[MAYA_ISO_TRIANGLE].get();
	SetModelRotate(pos, size);
	r.ExecuteDraw();
}

void MayaGraphics2d::DrawText(Font& font, MayaStringCR text, float x, float y, MayaCorner align)
{
	return DrawText(font, text, MayaFvec2(x, y), align);
}

void MayaGraphics2d::DrawText(Font& font, MayaStringCR text, MayaFvec2 pos, MayaCorner align)
{
	program->SetUniform<int>("uHasTexture[1]", 1);
	UpdateCameraOnDraw();
	r.Input = vao[MAYA_SQUARE].get();
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

		SetModelRotate(pos + tpos + d, glyph.Size);
		r.Textures[1] = glyph.Texture;
		r.ExecuteDraw();
		advance += glyph.Advance;
	}

	r.Textures[1] = 0;
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
	UpdateCameraOnDraw();
	r.Input = vao[MAYA_SQUARE].get();

	for (int i = start; i < end; i++)
	{
		program->SetUniformMatrix("uModel", text.global_model * text.char_models[i]);
		r.Textures[1] = (*text.font)[text.string[i]].Texture;
		r.ExecuteDraw();
	}

	r.Textures[1] = 0;
	program->SetUniform<int>("uHasTexture[1]", 0);
}

void MayaGraphics2d::DrawTick(float x, float y, float width, float height)
{
	DrawTick(MayaFvec2(x, y), MayaFvec2(width, height));
}

void MayaGraphics2d::DrawTick(MayaFvec2 pos, MayaFvec2 size)
{
	UpdateCameraOnDraw();
	r.Input = vao[MAYA_TICK].get();
	SetModelRotate(pos, size);
	r.ExecuteDraw();
}