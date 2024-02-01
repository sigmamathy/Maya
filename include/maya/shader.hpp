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

	MayaShaderProgram(MayaShaderProgram const&) = delete;

	MayaShaderProgram& operator=(MayaShaderProgram const&) = delete;

	template<class... Tys>
	void SetUniform(MayaStringCR name, Tys... args);

private:

	int programid;
	MayaWindow* window;
	MayaHashMap<MayaString, int> uniform_location_cache;
	friend class MayaRenderer;

	int FindUniformLocation(MayaStringCR name);
};