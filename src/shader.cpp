#include <maya/shader.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <fstream>

void MayaLoadShaderFromFile(MayaShaderProgramParameters& param, MayaStringCR target)
{
	std::ifstream ifs;

	ifs.open(target + ".vert.glsl");
	if (ifs) {
		MayaString line;
		while (getline(ifs, line))
			param.Vertex += line + '\n';
		ifs.close();
	}

	ifs.open(target + ".frag.glsl");
	if (ifs) {
		MayaString line;
		while (getline(ifs, line))
			param.Fragment += line + '\n';
		ifs.close();
	}

	ifs.open(target + ".geom.glsl");
	if (ifs) {
		MayaString line;
		while (getline(ifs, line))
			param.Geometry += line + '\n';
		ifs.close();
	}
}

static bool s_CreateShader(unsigned* result, unsigned type, MayaStringCR src, MayaString funcname)
{
	auto shader = glCreateShader(type);
	*result = shader;

	const char* cstr = src.c_str();
	glShaderSource(shader, 1, &cstr, nullptr);
	glCompileShader(shader);

	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		MayaString errmsg;
		errmsg.resize(512);
		glGetShaderInfoLog(shader, 512, NULL, &errmsg[0]);

		switch (type) {
			case GL_VERTEX_SHADER: errmsg = "Following error found in vertex shader\n" + errmsg; break;
			case GL_FRAGMENT_SHADER: errmsg = "Following error found in fragment shader\n" + errmsg; break;
			case GL_GEOMETRY_SHADER: errmsg = "Following error found in geometry shader\n" + errmsg; break;
		}

		MAYA_SERR(MAYA_SHADER_COMPILE_ERROR, funcname + errmsg);
		return false;
	}

	return true;
}

template<class Ty>
static Ty s_CreateShaderProgramPtr(MayaWindow& window, MayaShaderProgramParameters& param)
{
	window.UseGraphicsContext();
	unsigned programid = glCreateProgram();
	unsigned vertex = 0, fragment = 0, geometry = 0;

	auto const funcname = std::is_same_v<Ty, MayaShaderProgramUptr>
		? "MayaCreateShaderProgramUptr(MayaWindow&, MayaShaderProgramParameters&): "
		: "MayaCreateShaderProgramSptr(MayaWindow&, MayaShaderProgramParameters&): ";
	bool noerr = true;

	if (!param.Vertex.empty()) {
		if (noerr = s_CreateShader(&vertex, GL_VERTEX_SHADER, param.Vertex, funcname) && noerr)
			glAttachShader(programid, vertex);
	}

	if (!param.Fragment.empty()) {
		if (noerr = s_CreateShader(&fragment, GL_FRAGMENT_SHADER, param.Fragment, funcname) && noerr)
			glAttachShader(programid, fragment);
	}

	if (!param.Geometry.empty()) {
		if (noerr = s_CreateShader(&geometry, GL_GEOMETRY_SHADER, param.Geometry, funcname) && noerr)
			glAttachShader(programid, geometry);
	}

	glLinkProgram(programid);

	if (vertex)		glDeleteShader(vertex);
	if (fragment)	glDeleteShader(fragment);
	if (geometry)	glDeleteShader(geometry);

	if (!noerr) {
		glDeleteProgram(programid);
		return nullptr;
	}

	if constexpr (std::is_same_v<Ty, MayaShaderProgramUptr>) return std::make_unique<MayaShaderProgram>(programid, &window);
	else return std::make_shared<MayaShaderProgram>(programid, &window);
}

MayaShaderProgramUptr MayaCreateShaderProgramUptr(MayaWindow& window, MayaShaderProgramParameters& param)
{
	return s_CreateShaderProgramPtr<MayaShaderProgramUptr>(window, param);
}

MayaShaderProgramSptr MayaCreateShaderProgramSptr(MayaWindow& window, MayaShaderProgramParameters& param)
{
	return s_CreateShaderProgramPtr<MayaShaderProgramSptr>(window, param);
}

MayaShaderProgram::MayaShaderProgram(int program, MayaWindow* window)
	: programid(program), window(window)
{
}

MayaShaderProgram::~MayaShaderProgram()
{
	glDeleteProgram(programid);
}