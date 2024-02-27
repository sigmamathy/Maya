#pragma once

#include "./graphics.hpp"
#include "./button.hpp"

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

	void SetScrollView(float view);

	float GetScrollView() const;

	void SetValue(float value);

	float GetValue() const;

private:

	float value, view, max;
	Direction direction;
	bool cursor_clicked;
	MayaFvec2 cursor_prev_pos;
	float prev_value;

	void ButtonPosAndSize(int index, MayaFvec2& pos, MayaFvec2& size);
	void SlidePosAndSize(MayaFvec2& pos, MayaFvec2& size);
};