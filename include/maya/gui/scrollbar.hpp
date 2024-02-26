#pragma once

#include "./graphics.hpp"

class MayaScrollBarGui : public MayaComponentGui
{
public:

	enum Direction : bool
	{
		Horizontal,
		Vertical
	};

	MayaScrollBarGui(MayaGraphicsGui& gui);

	void ReactEvent(MayaEvent& e) override;

	void Draw(MayaGraphics2d& g2d) override;

	void SetScrollDirection(Direction dir);

	Direction GetScrollDirection() const;

	void SetScrollMax(float max);

	float GetScrollMax() const;

	void SetValue(float value);

	float GetValue() const;

private:

	float value, max;
	Direction direction;
	bool cursor_clicked;
	MayaFvec2 cursor_prev_pos;
	float prev_value;
};