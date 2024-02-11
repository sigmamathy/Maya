#pragma once

#include "./graphics.hpp"
#include "../2d/textdisplay.hpp"

class MayaGraphicsGUI::Checkbox : public MayaGraphicsGUI::Component
{
public:

	Checkbox(MayaGraphicsGUI& gui);

	virtual void Draw(MayaGraphics2D& g2d) override;

	void ReactEvent(MayaEvent& e) override;

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

	void SetSelected(bool select);

	bool IsSelected() const;

	bool IsCheckboxTouched() const;

private:

	MayaGraphics2D::TextDisplay text;
	bool selected;
};