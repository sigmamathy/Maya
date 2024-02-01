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
	window->UseGraphicsContext();
	glDeleteProgram(programid);
}

unsigned Maya_s_binded_shader_program = 0;

static void s_BindShaderProgram(unsigned id)
{
	if (id == Maya_s_binded_shader_program) return;
	glUseProgram(id);
	Maya_s_binded_shader_program = id;
}

int MayaShaderProgram::FindUniformLocation(MayaStringCR name)
{
	window->UseGraphicsContext();
	if (uniform_location_cache.count(name))
		return uniform_location_cache.at(name);
	int x = glGetUniformLocation(programid, name.c_str());
	uniform_location_cache[name] = x;
	return x;
}

template<>
void MayaShaderProgram::SetUniform(MayaStringCR name, float x, float y, float z, float w)
{	
	window->UseGraphicsContext();
	s_BindShaderProgram(programid);
	int loc = FindUniformLocation(name);
	glUniform4f(loc, x, y, z, w);
}