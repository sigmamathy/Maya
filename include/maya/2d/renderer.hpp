#pragma once

#include "./graphics.hpp"

class MayaGraphics2D::Renderer
{
public:

	Renderer(MayaGraphics2D& g2d);

private:

	MayaGraphics2D* g2d;
	MayaTexture* texture;
};