#include <maya/2d/renderer.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>

MayaGraphics2D::Renderer::Renderer(MayaGraphics2D& g2d)
	: g2d(&g2d), texture(nullptr)
{
}

