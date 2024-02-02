#pragma once

#include "./core.hpp"
#include "./math.hpp"

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

	MayaShaderProgram(unsigned program, MayaWindow* window);

	~MayaShaderProgram();

	MayaShaderProgram(MayaShaderProgram const&) = delete;

	MayaShaderProgram& operator=(MayaShaderProgram const&) = delete;

	template<class Ty, class... Tys> requires (std::is_same_v<Ty, Tys> && ...)
	void SetUniform(MayaStringCR name, Tys... args);

	template<class Ty, int Sz>
	void SetUniformVector(MayaStringCR name, MayaVector<Ty, Sz> const& vec);

	template<int Rw, int Cn>
	void SetUniformMatrix(MayaStringCR name, MayaMatrix<float, Rw, Cn> const& mat);

private:

	int programid;
	MayaWindow* window;
	MayaHashMap<MayaString, int> uniform_location_cache;
	friend class MayaRenderer;

	int FindUniformLocation(MayaStringCR name);
};

template<class Ty, class... Tys> requires (std::is_same_v<Ty, Tys> && ...)
void MayaShaderProgram::SetUniform(MayaStringCR name, Tys... args)
{
	MayaVector<Ty, sizeof...(Tys)> vec = { args... };
	SetUniformVector(name, vec);
}