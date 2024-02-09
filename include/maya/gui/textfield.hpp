#pragma once

#include "./graphics.hpp"

class MayaGraphicsGUI::TextField : public MayaGraphicsGUI::Component
{
public:

	TextField(MayaGraphicsGUI& gui);

	virtual void Draw(MayaGraphics2D& g2d) override;

	void ReactEvent(MayaEvent& e) override;
};