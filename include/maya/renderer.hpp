#pragma once

#include "./core.hpp"
#include "./shader.hpp"
#include "./vertexarray.hpp"
#include "./texture.hpp"

enum MayaPerFragTest
{
	MayaNoTest = 0,
	MayaScissorTest = 1,
};

void MayaSetScissorRect(MayaWindow* window, MayaFvec2 pos, MayaFvec2 size);

class MayaRenderer
{
public:

	MayaVertexArray* Input			= 0; // REQUIRED
	MayaShaderProgram* Program		= 0; // REQUIRED
	MayaTexture* Textures[16]		= {0};
	
	MayaPerFragTest Test			= MayaNoTest;

	void ExecuteDraw();
};