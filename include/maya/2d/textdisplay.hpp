#pragma once

#include "./graphics.hpp"

class MayaGraphics2D::TextDisplay
{
public:

	TextDisplay(MayaFont& font, MayaStringCR text);
	TextDisplay& operator=(MayaStringCR text);

	MayaStringCR GetString() const;
	unsigned GetLength() const;

	void SetTextAlign(MayaCorner align);
	MayaCorner GetTextAlign() const;

	void SetPosition(MayaFvec2 pos);
	void SetScale(MayaFvec2 scale);
	void SetRotation(float rot);

	MayaFvec2 GetPosition() const;
	MayaFvec2 GetScale() const;
	float GetRotation() const;

private:

	MayaFont* font;
	MayaString string;
	MayaFvec2 text_size;
	MayaCorner text_align;

	MayaFvec2 position;
	MayaFvec2 scale;
	float rotation;

	bool require_update_global_model;
	MayaFmat4 global_model;
	MayaArrayList<MayaFmat4> char_models;

	void ComputeGlobalModel();
	friend class MayaGraphics2D;
};