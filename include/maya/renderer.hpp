#pragma once

#include "./core.hpp"
#include "./shader.hpp"
#include "./vertexarray.hpp"

class MayaRenderer
{
public:
	MayaVertexArray* Input = 0;
	MayaShaderProgram* Program = 0;
	void ExecuteDraw();
};