#pragma once

#include "./graphics.hpp"

class MayaPanelGui : public MayaContainerGui
{
public:

	MayaPanelGui(MayaGraphicsGui& gui);

	void ReactEvent(MayaEvent& e) override;
	void Draw(MayaGraphics2d& g2d) override;
};