#include <maya/gui/graphics.hpp>
#include <maya/gui/button.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>
#include <maya/font.hpp>

#include "./opensans.h"

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
uniform int uHasTexture;
uniform sampler2D uTexture;

void main()
{
	oFragColor = uColor;
	if (uHasTexture == 1)
		oFragColor = oFragColor * texture(uTexture, vTexCoord);
}

)";

MayaGraphicsGUI::MayaGraphicsGUI(MayaWindow* window)
	: Window(window), texture(0)
{
	MayaShaderProgramParameters param;
	param.Vertex = s_vertex_shader;
	param.Fragment = s_fragment_shader;
	program = MayaCreateShaderProgramUptr(*window, param);

	program->SetUniformMatrix("uModel", MayaFmat4(1));
	UpdateProjection();
	program->SetUniformVector("uColor", MayaWhite / 255.0f);

	program->SetUniform<int>("uHasTexture", 0);
	program->SetUniform<int>("uTexture", 0);

	static float square_vertices[] = {
		0, 0,		0, 1,
		0, 1,		0, 0,
		1, 0,		1, 1,
		0, 1,		0, 0,
		1, 0,		1, 1,
		1, 1,		1, 0
	};

	squarevao = MayaCreateVertexArrayUptr(*window);
	squarevao->SetVertexCount(6);
	MayaVertexLayout layout;
	layout(0, 2) (1, 2);
	layout.Data = square_vertices;
	layout.Size = sizeof(square_vertices);
	squarevao->LinkVertexBuffer(layout);
}

void MayaGraphicsGUI::UpdateProjection()
{
	auto sz = Window->GetSize();
	MayaFmat4 proj = MayaOrthogonalProjection(0, (float)sz.x, (float)sz.y, 0);
	program->SetUniformMatrix("uProjection", proj);
}

MayaGraphicsGUI::Button& MayaGraphicsGUI::CreateButton()
{
	auto button = new Button(*this);
	components.emplace_back(static_cast<Component*>(button));
	return *button;
}

void MayaGraphicsGUI::Draw()
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i])
			components[i]->Draw();
	}
}

MayaFont* MayaGraphicsGUI::GetOpenSansRegularFont()
{
	if (!opensans_font) 
		opensans_font = MayaCreateFontUptr(*Window, s_OpenSansRegular, sizeof(s_OpenSansRegular), 40);
	return opensans_font.get();
}

MayaGraphicsGUI::Component::Component(MayaGraphicsGUI& gui)
	: position(0), size(50), gui(&gui), color0(MayaWhite), color1(MayaBlack)
{
}

void MayaGraphicsGUI::Component::SetPosition(MayaFvec2 pos)
{
	position = pos;
}

void MayaGraphicsGUI::Component::SetSize(MayaFvec2 size)
{
	this->size = size;
}

MayaFvec2 MayaGraphicsGUI::Component::GetPosition() const
{
	return position;
}

MayaFvec2 MayaGraphicsGUI::Component::GetSize() const
{
	return size;
}

void MayaGraphicsGUI::Component::SetColor0(MayaIvec4 color)
{
	color0 = color;
}

void MayaGraphicsGUI::Component::SetColor1(MayaIvec4 color)
{
	color1 = color;
}

MayaIvec4 MayaGraphicsGUI::Component::GetColor0() const
{
	return color0;
}

MayaIvec4 MayaGraphicsGUI::Component::GetColor1() const
{
	return color1;
}

void MayaGraphicsGUI::Component::UseColor(MayaIvec4 color)
{
	gui->program->SetUniformVector("uColor", color / 255.0f);
}

void MayaGraphicsGUI::Component::UseTexture(MayaTexture* texture)
{
	gui->texture = texture;
	gui->program->SetUniform<int>("uHasTexture", texture ? 1 : 0);
}

void MayaGraphicsGUI::Component::DrawRect(MayaFvec2 pos, MayaFvec2 size)
{
	gui->program->SetUniformMatrix("uModel", MayaTranslate(pos) * MayaScale(size));
	MayaRenderer r;
	r.Input = gui->squarevao.get();
	r.Program = gui->program.get();
	r.Textures[0] = gui->texture;
	r.ExecuteDraw();
}

void MayaGraphicsGUI::Component::DrawText(MayaFont* font, MayaStringCR text, MayaFvec2 pos, MayaTextAlign align)
{
	if (!font)
		font = gui->GetOpenSansRegularFont();
	if (!gui->texture)
		gui->program->SetUniform<int>("uHasTexture", 1);

	MayaFvec2 tsize(0);

	if (align != MayaTextAlignBL) {
		for (int i = 0, advance = 0; i < text.size(); i++)
		{
			auto& glyph = (*font)[text[i]];
			tsize.x += glyph.Advance;
			if (glyph.Bearing.y > tsize.y)
				tsize.y = static_cast<float>(glyph.Bearing[1]);
			advance += glyph.Advance;
		}
	}

	int advance = 0;
	for (char c : text)
	{
		auto& glyph = (*font)[c];
		MayaFvec2 tpos = {
			pos.x + advance + glyph.Bearing.x,
			pos.y - glyph.Bearing.y
		};

		MayaFvec2 d;
		d.x = -tsize.x * ((align & 0b11) - 1) * 0.5f;
		d.y = tsize.y * ((align >> 2) - 1) * 0.5f;

		gui->program->SetUniformMatrix("uModel", MayaTranslate(tpos + d) * MayaScale(glyph.Size));
		MayaRenderer r;
		r.Input = gui->squarevao.get();
		r.Program = gui->program.get();
		r.Textures[0] = glyph.Texture;
		r.ExecuteDraw();
		advance += glyph.Advance;
	}
	if (!gui->texture)
		gui->program->SetUniform<int>("uHasTexture", 0);
}