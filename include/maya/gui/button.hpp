#pragma once

#include "./graphics.hpp"
#include "../2d/textdisplay.hpp"

class MayaButtonGui : public MayaComponentGui
{
public:

	enum ButtonIcon {
		NoIcon,
		TriangleUp,
		TriangleDown,
		Tick,
	};

	MayaButtonGui(MayaGraphicsGui& gui);

	virtual void Draw(MayaGraphics2d& g2d) override;

	void ReactEvent(MayaEvent& e) override;

	void SetText(MayaStringCR text);

	MayaStringCR GetText() const;

	void SetButtonIcon(ButtonIcon icon);

	ButtonIcon GetButtonIcon() const;

	bool IsButtonPressed() const;

private:

	MayaTextDisplay2d text;
	bool pressed;
	ButtonIcon icon;
};