#pragma once

#include "./graphics.hpp"

class MayaGraphicsGUI::Button : public MayaGraphicsGUI::Component
{
public:

	Button(MayaGraphicsGUI& gui);

	virtual void Draw(MayaGraphics2D& g2d) override;

	void ReactEvent(MayaEvent& e) override;

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

	bool IsButtonTouched() const;

	bool IsButtonPressed() const;

private:

	MayaString text;
	bool pressed;
};