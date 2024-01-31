#pragma once

#include "./core.hpp"

struct MayaShaderProgramParameters
{
	MayaString Vertex;
	MayaString Fragment;
	MayaString Geometry;
};

class MayaWindow;
MAYA_TYPEDEFPTR(MayaShaderProgram);

void MayaLoadShaderFromFile(MayaShaderProgramParameters& param, MayaStringCR target);

MayaShaderProgramUptr MayaCreateShaderProgramUptr(MayaWindow& window, MayaShaderProgramParameters& param);

MayaShaderProgramSptr MayaCreateShaderProgramSptr(MayaWindow& window, MayaShaderProgramParameters& param);

class MayaShaderProgram
{
public:

	MayaShaderProgram(int program, MayaWindow* window);

	~MayaShaderProgram();

private:

	int programid;
	MayaWindow* window;

};