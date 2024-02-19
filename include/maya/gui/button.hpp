#pragma once

#include "./graphics.hpp"
#include "../2d/textdisplay.hpp"

class MayaButtonGui : public MayaComponentGui
{
public:

	MayaButtonGui(MayaGraphicsGui& gui);

	virtual void Draw(MayaGraphics2d& g2d) override;

	void ReactEvent(MayaEvent& e) override;

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

	bool IsButtonTouched() const;

	bool IsButtonPressed() const;

private:

	MayaTextDisplay2d text;
	bool pressed;
};