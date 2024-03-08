#include <maya/2d/textdisplay.hpp>
#include <maya/transformation.hpp>

MayaTextDisplay2d::MayaTextDisplay2d(Font& font, MayaStringCR text)
	: font(&font), string(""), require_update_char(false), text_size(0),
	text_align(MayaCornerBL), global_model(1), require_update_global(0),
	position(0), scale(1), rotation(0)
{
	if (!text.empty())
		operator=(text);
}

MayaTextDisplay2d& MayaTextDisplay2d::operator=(MayaStringCR text)
{
	string = text;
	require_update_char = true;
	return *this;
}

void MayaTextDisplay2d::InsertCharAt(int index, char c)
{
	string.insert(index, 1, c);
	require_update_char = true;
}

char MayaTextDisplay2d::RemoveCharAt(int index)
{
	char c = string[index];
	string.erase(index, 1);
	require_update_char = true;
	return c;
}

void MayaTextDisplay2d::InsertStringAt(int index, MayaStringCR str)
{
	string.insert(index, str);
	require_update_char = true;
}

void MayaTextDisplay2d::SetFont(Font& font)
{
	if (this->font != &font)
	{
		this->font = &font;
		require_update_char = true;
	}
}

Font& MayaTextDisplay2d::GetFont() const
{
	return *font;
}

MayaStringCR MayaTextDisplay2d::GetString() const
{
	return string;
}

unsigned MayaTextDisplay2d::GetLength() const
{
	return (unsigned) string.length();
}

void MayaTextDisplay2d::SetTextAlign(MayaCorner align)
{
	text_align = align;
	require_update_global = true;
}

MayaCorner MayaTextDisplay2d::GetTextAlign() const
{
	return text_align;
}

void MayaTextDisplay2d::SetPosition(MayaFvec2 pos)
{
	this->position = pos;
	require_update_global = true;
}

void MayaTextDisplay2d::SetScale(MayaFvec2 scale)
{
	this->scale = scale;
	require_update_global = true;
}

void MayaTextDisplay2d::SetRotation(float rot)
{
	this->rotation = rot;
	require_update_global = true;
}

MayaFvec2 MayaTextDisplay2d::GetPosition() const
{
	return position;
}

MayaFvec2 MayaTextDisplay2d::GetScale() const
{
	return scale;
}

float MayaTextDisplay2d::GetRotation() const
{
	return rotation;
}

void MayaTextDisplay2d::ComputeGlobalModel()
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

void MayaTextDisplay2d::ComputeCharModel()
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