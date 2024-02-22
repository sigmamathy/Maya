#pragma once

#include "./core.hpp"
#include "./shader.hpp"
#include "./vertexarray.hpp"
#include "./texture.hpp"
#include "./fragtest.hpp"

class MayaRenderer
{
public:

	MayaVertexArray* Input			= 0; // REQUIRED
	MayaShaderProgram* Program		= 0; // REQUIRED
	MayaTexture* Textures[16]		= {0};
	
	MayaBlending* Blending = 0;
	MayaScissorTest* Scissor = 0;

	void ExecuteDraw();
};