#include <maya/2d/textdisplay.hpp>
#include <maya/transformation.hpp>

MayaGraphics2D::TextDisplay::TextDisplay(MayaFont& font, MayaStringCR text)
	: font(&font),
	text_align(MayaCornerBL), global_model(1), require_update_global(0),
	position(0), scale(1), rotation(0)
{
	operator=(text);
}

MayaGraphics2D::TextDisplay& MayaGraphics2D::TextDisplay::operator=(MayaStringCR text)
{
	string = text;
	require_update_char = true;
	return *this;
}

void MayaGraphics2D::TextDisplay::InsertCharAt(int index, char c)
{
	string.insert(index, 1, c);
	require_update_char = true;
}

char MayaGraphics2D::TextDisplay::RemoveCharAt(int index)
{
	char c = string[index];
	string.erase(index, 1);
	require_update_char = true;
	return c;
}

MayaFont& MayaGraphics2D::TextDisplay::GetFont() const
{
	return *font;
}

MayaStringCR MayaGraphics2D::TextDisplay::GetString() const
{
	return string;
}

unsigned MayaGraphics2D::TextDisplay::GetLength() const
{
	return (unsigned) string.length();
}

void MayaGraphics2D::TextDisplay::SetTextAlign(MayaCorner align)
{
	text_align = align;
	require_update_global = true;
}

MayaCorner MayaGraphics2D::TextDisplay::GetTextAlign() const
{
	return text_align;
}

void MayaGraphics2D::TextDisplay::SetPosition(MayaFvec2 pos)
{
	this->position = pos;
	require_update_global = true;
}

void MayaGraphics2D::TextDisplay::SetScale(MayaFvec2 scale)
{
	this->scale = scale;
	require_update_global = true;
}

void MayaGraphics2D::TextDisplay::SetRotation(float rot)
{
	this->rotation = rot;
	require_update_global = true;
}

MayaFvec2 MayaGraphics2D::TextDisplay::GetPosition() const
{
	return position;
}

MayaFvec2 MayaGraphics2D::TextDisplay::GetScale() const
{
	return scale;
}

float MayaGraphics2D::TextDisplay::GetRotation() const
{
	return rotation;
}

void MayaGraphics2D::TextDisplay::ComputeGlobalModel()
{
	ComputeCharModel();
	if (require_update_global) {
		MayaFvec2 d;
		d.x = -text_size.x * ((text_align & 0b11) - 1) * 0.5f;
		d.y = -text_size.y * ((text_align >> 2) - 1) * 0.5f;
		global_model = MayaTranslate(position) * MayaRotate(rotation) * MayaScale(scale) * MayaTranslate(d);
		require_update_global = false;
	}
}

void MayaGraphics2D::TextDisplay::ComputeCharModel()
{
	if (require_update_char)
	{
		char_models.clear();
		text_size = MayaFvec2(0);
		char_models.reserve(string.size());

		for (int i = 0, advance = 0; i < string.size(); i++)
		{
			auto& glyph = (*font)[string[i]];
			text_size.x += glyph.Advance;
			if (glyph.Bearing.y > text_size.y)
				text_size.y = static_cast<float>(glyph.Bearing.y);

			MayaFvec2 tpos = {
				advance + glyph.Bearing.x + glyph.Size.x * 0.5f,
				glyph.Bearing.y - glyph.Size.y + glyph.Size.y * 0.5f
			};

			char_models.emplace_back(MayaTranslate(tpos) * MayaScale(glyph.Size));
			advance += glyph.Advance;
		}

		require_update_char = false;
	}
}