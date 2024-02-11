#pragma once

#include "./core.hpp"
#include "./shader.hpp"
#include "./vertexarray.hpp"
#include "./texture.hpp"

enum MayaPerFragTest : unsigned
{
	MayaNoTest			= 0,
	MayaScissorTest		= 1,
	MayaBlending		= 2
};

void MayaSetScissorRect(MayaWindow* window, MayaFvec2 pos, MayaFvec2 size);

class MayaRenderer
{
public:

	MayaVertexArray* Input			= 0; // REQUIRED
	MayaShaderProgram* Program		= 0; // REQUIRED
	MayaTexture* Textures[16]		= {0};
	
	unsigned Test			= MayaNoTest;

	void ExecuteDraw();
};