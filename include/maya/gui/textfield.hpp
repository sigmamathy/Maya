#pragma once

#include "./graphics.hpp"
#include "../2d/textdisplay.hpp"

class MayaTextFieldGui : public MayaComponentGui
{
public:

	MayaTextFieldGui(MayaGraphicsGui& gui);

	virtual void Draw(MayaGraphics2d& g2d) override;

	void ReactEvent(MayaEvent& e) override;

	void SetFont(MayaFont* font);

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

	void SetDescription(MayaStringCR desc);

	MayaStringCR GetDescription() const;

	bool IsTextFieldTouched() const;

private:

	void SetCaretPosToMousePos();

private:

	MayaTextDisplay2d text, description;
	int careti, caretpos;
	float caret_timer;
	int scroll = 0;
};