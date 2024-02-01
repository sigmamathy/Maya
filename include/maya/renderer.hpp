#pragma once

#include "./core.hpp"
#include "./shader.hpp"
#include "./vertexarray.hpp"

class MayaRenderer
{
public:
	MayaVertexArray* Input;
	MayaShaderProgram* Program;
	void ExecuteDraw();
};