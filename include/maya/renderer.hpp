#pragma once

#include "./core.hpp"
#include "./shader.hpp"
#include "./vertexarray.hpp"
#include "./texture.hpp"

enum MayaPerFragTest : unsigned
{
	MayaNoTest			= 0x00,
	MayaScissorTest		= 0x01,
	MayaBlending		= 0x02
};

void MayaPushScissorRect(MayaWindow* window, MayaIvec2 pos, MayaIvec2 size);
void MayaPopScissorRect(MayaWindow* window);

class MayaRenderer
{
public:

	MayaVertexArray* Input			= 0; // REQUIRED
	MayaShaderProgram* Program		= 0; // REQUIRED
	MayaTexture* Textures[16]		= {0};
	
	unsigned Test			= MayaNoTest;

	void ExecuteDraw();
};