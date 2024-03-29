#pragma once

#include "./graphics.hpp"
#include "../2d/textdisplay.hpp"

class MayaLabelGui : public MayaComponentGui
{
public:

	MayaLabelGui(MayaGraphicsGui& gui);

	virtual void Draw(MayaGraphics2d& g2d) override;

	void ReactEvent(MayaEvent& e) override;

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

private:

	MayaTextDisplay2d text;
};