#pragma once

#include "./graphics.hpp"
#include "../2d/textdisplay.hpp"

class MayaCheckboxGui : public MayaComponentGui
{
public:

	MayaCheckboxGui(MayaGraphicsGui& gui);

	virtual void Draw(MayaGraphics2d& g2d) override;

	void ReactEvent(MayaEvent& e) override;

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

	void SetSelected(bool select);

	bool IsSelected() const;

	bool IsCheckboxTouched() const;

private:

	MayaTextDisplay2d text;
	bool selected;
};