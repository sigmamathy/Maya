#include <maya/2d/textdisplay.hpp>
#include <maya/transformation.hpp>

MayaGraphics2D::TextDisplay::TextDisplay(MayaFont& font, MayaStringCR text)
	: font(&font), string(text), text_size(0), text_align(MayaTextAlignBL), global_model(1), require_update_global_model(0),
	position(0), scale(1), rotation(0)
{
	char_models.reserve(text.size());

	for (int i = 0, advance = 0; i < text.size(); i++)
	{
		auto& glyph = font[text[i]];
		text_size.x += glyph.Advance;
		if (glyph.Bearing.y > text_size.y)
			text_size.y = static_cast<float>(glyph.Bearing[1]);

		MayaFvec2 tpos = {
			advance + glyph.Bearing.x + glyph.Size.x * 0.5f,
			glyph.Bearing.y - glyph.Size.y + glyph.Size.y * 0.5f
		};

		char_models.emplace_back(MayaTranslate(tpos) * MayaScale(glyph.Size));
		advance += glyph.Advance;
	}

}

MayaStringCR MayaGraphics2D::TextDisplay::GetString() const
{
	return string;
}

unsigned MayaGraphics2D::TextDisplay::GetLength() const
{
	return (unsigned) string.length();
}

void MayaGraphics2D::TextDisplay::SetTextAlign(MayaTextAlign align)
{
	text_align = align;
	require_update_global_model = true;
}

MayaTextAlign MayaGraphics2D::TextDisplay::GetTextAlign() const
{
	return text_align;
}

void MayaGraphics2D::TextDisplay::SetPosition(MayaFvec2 pos)
{
	this->position = pos;
	require_update_global_model = true;
}

void MayaGraphics2D::TextDisplay::SetScale(MayaFvec2 scale)
{
	this->scale = scale;
	require_update_global_model = true;
}

void MayaGraphics2D::TextDisplay::SetRotation(float rot)
{
	this->rotation = rot;
	require_update_global_model = true;
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
	if (require_update_global_model) {
		MayaFvec2 d;
		d.x = -text_size.x * ((text_align & 0b11) - 1) * 0.5f;
		d.y = -text_size.y * ((text_align >> 2) - 1) * 0.5f;
		global_model = MayaTranslate(position) * MayaRotate(rotation) * MayaScale(scale) * MayaTranslate(d);
		require_update_global_model = false;
	}
}