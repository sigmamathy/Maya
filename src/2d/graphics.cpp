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

MayaGraphics2D::MayaGraphics2D(MayaWindow& window)
	: Window(&window), texture(nullptr), camera(nullptr), projection(1), has_scissor(false)
{
	MayaShaderProgramParameters param;
	param.Vertex = s_vertex_shader;
	param.Fragment = s_fragment_shader;
	program = MayaCreateShaderProgramUptr(window, param);
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

	squarevao = MayaCreateVertexArrayUptr(window);
	squarevao->SetVertexCount(6);
	MayaVertexLayout layout;
	layout (0, 2) (1, 2);
	layout.Data = square_vertices;
	layout.Size = sizeof(square_vertices);
	squarevao->LinkVertexBuffer(layout);

	circlevao64 = s_CreateCircleVertexArray(window, 64);
}

void MayaGraphics2D::UseProjection(float width, float height)
{
	UseProjection(MayaFvec2(width, height));
}

void MayaGraphics2D::UseProjection(MayaFvec2 size)
{
	MayaFmat4 proj = MayaOrthogonalProjection(size);
	program->SetUniformMatrix("uProjection", proj);
	projection = size;
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

void MayaGraphics2D::BeginScissor(MayaFvec2 pos, MayaFvec2 size)
{
	MayaFvec2 wsz = Window->GetSize();
	float xs = wsz.x / projection.x, ys = wsz.y / projection.y;
	pos.x *= xs;
	pos.y *= ys;
	size.x *= xs;
	size.y *= ys;
	pos = pos + wsz * 0.5f - size * 0.5f;
	pos.y = wsz.y - pos.y - size.y;
	MayaSetScissorRect(Window, pos, size);
	has_scissor = true;
}

void MayaGraphics2D::EndScissor()
{
	has_scissor = false;
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
	r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
	r.ExecuteDraw();
}

void MayaGraphics2D::DrawRectBorder(float x, float y, float width, float height, int linewidth)
{
	DrawRectBorder(MayaFvec2(x, y), MayaFvec2(width, height), linewidth);
}

void MayaGraphics2D::DrawRectBorder(MayaFvec2 pos, MayaFvec2 size, int linewidth)
{
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	program->SetUniform<int>("uHasTexture[0]", 0);
	MayaRenderer r;
	r.Input = squarevao.get();
	r.Program = program.get();
	r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
	program->SetUniformMatrix("uModel", MayaTranslate(MayaFvec2(pos.x - size.x * 0.5f, pos.y))
		* MayaScale(MayaFvec2(linewidth, size.y)));
	r.ExecuteDraw();
	program->SetUniformMatrix("uModel", MayaTranslate(MayaFvec2(pos.x + size.x * 0.5f, pos.y))
		* MayaScale(MayaFvec2(linewidth, size.y)));
	r.ExecuteDraw();
	program->SetUniformMatrix("uModel", MayaTranslate(MayaFvec2(pos.x, pos.y + size.y * 0.5f))
		* MayaScale(MayaFvec2(size.x, linewidth)));
	r.ExecuteDraw();
	program->SetUniformMatrix("uModel", MayaTranslate(MayaFvec2(pos.x, pos.y - size.y * 0.5f))
		* MayaScale(MayaFvec2(size.x, linewidth)));
	r.ExecuteDraw();
	program->SetUniform<int>("uHasTexture[0]", texture ? 1 : 0);
}

void MayaGraphics2D::DrawLine(float startx, float starty, float endx, float endy)
{
	DrawLine(MayaFvec2(startx, starty), MayaFvec2(endx, endy));
}

void MayaGraphics2D::DrawLine(MayaFvec2 start, MayaFvec2 end)
{
	if (start == end) return;
	MayaFmat4 pos = MayaTranslate((start + end) / 2.0f);
	MayaFvec2 dv = end - start;
	MayaFmat4 scale = MayaScale(MayaFvec2(dv.Norm(), 1));
	MayaFmat4 rot = MayaRotate(std::atan2(dv[1], dv[0]));
	program->SetUniformMatrix("uModel", pos * rot * scale);
	program->SetUniform<int>("uHasTexture[0]", 0);
	MayaRenderer r;
	r.Input = squarevao.get();
	r.Program = program.get();
	r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
	r.ExecuteDraw();
	program->SetUniform<int>("uHasTexture[0]", texture ? 1 : 0);
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
	r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
	r.ExecuteDraw();
}

void MayaGraphics2D::DrawText(MayaFont& font, MayaStringCR text, float x, float y, MayaCorner align)
{
	return DrawText(font, text, MayaFvec2(x, y), align);
}

void MayaGraphics2D::DrawText(MayaFont& font, MayaStringCR text, MayaFvec2 pos, MayaCorner align)
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
		r.Input = squarevao.get();
		r.Program = program.get();
		r.Textures[0] = texture;
		r.Textures[1] = glyph.Texture;
		r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
		r.ExecuteDraw();
		advance += glyph.Advance;
	}
	program->SetUniform<int>("uHasTexture[1]", 0);
}

void MayaGraphics2D::DrawText(TextDisplay& text)
{
	DrawText(text, 0, text.GetLength());
}

void MayaGraphics2D::DrawText(TextDisplay& text, int start, int end)
{
	text.ComputeGlobalModel();
	program->SetUniform<int>("uHasTexture[1]", 1);
	if (camera && camera->require_update)
		program->SetUniformMatrix("uView", camera->GetViewMatrix());
	for (int i = start; i < end; i++)
	{
		program->SetUniformMatrix("uModel", text.global_model * text.char_models[i]);
		MayaRenderer r;
		r.Input = squarevao.get();
		r.Program = program.get();
		r.Textures[0] = texture;
		r.Textures[1] = (*text.font)[text.string[i]].Texture;
		r.Test = has_scissor ? MayaScissorTest : MayaNoTest;
		r.ExecuteDraw();
	}
	program->SetUniform<int>("uHasTexture[1]", 0);
}