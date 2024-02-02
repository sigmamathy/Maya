#pragma once

#include <maya/2d/graphics.hpp>

struct Maya_s_Graphics2D_Resources
{
	MayaShaderProgramUptr Program;
	MayaVertexArrayUptr SquareVAO;
};

extern MayaHashMap<MayaWindow*, Maya_s_Graphics2D_Resources> Maya_s_g2d_resources;