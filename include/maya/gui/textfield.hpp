#pragma once

#include "./graphics.hpp"
#include "../2d/textdisplay.hpp"

class MayaGraphicsGUI::TextField : public MayaGraphicsGUI::Component
{
public:

	TextField(MayaGraphicsGUI& gui);

	virtual void Draw(MayaGraphics2D& g2d) override;

	void ReactEvent(MayaEvent& e) override;

	void SetFont(MayaFont* font);

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

	void SetDescription(MayaStringCR desc);

	MayaStringCR GetDescription() const;

	bool IsTextFieldTouched() const;

private:

	void UpdateCaretPos();

private:

	MayaGraphics2D::TextDisplay text, description;
	int careti, caretpos;
	float caret_timer;
	int scroll = 0;
};