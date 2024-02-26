#pragma once

#include "./graphics.hpp"
#include "../2d/textdisplay.hpp"

class MayaNumberScrollGui : public MayaComponentGui
{
public:

	MayaNumberScrollGui(MayaGraphicsGui& gui);

	void ReactEvent(MayaEvent& e) override;

	void Draw(MayaGraphics2d& g2d) override;

	void SetRange(float min, float max);

	MayaFvec2 GetRange() const;

	void SetValue(float value);

	float GetValue() const;

	void SetPrecision(int num);

	int GetPrecision() const;

private:

	MayaFvec2 range;
	float value;
	MayaTextDisplay2d text;
	bool cursor_clicked;
	int precision;
};