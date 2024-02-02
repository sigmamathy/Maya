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

MayaShaderProgram::MayaShaderProgram(unsigned program, MayaWindow* window)
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

#define MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(ty, sz, fn)\
	template<> void MayaShaderProgram::SetUniformVector(MayaStringCR name, MayaVector<ty, sz> const& vec)\
	{ window->UseGraphicsContext(); s_BindShaderProgram(programid); int loc = FindUniformLocation(name); fn(loc, 1, &vec[0]); }

MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(float, 1, glUniform1fv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(float, 2, glUniform2fv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(float, 3, glUniform3fv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(float, 4, glUniform4fv)

MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(int, 1, glUniform1iv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(int, 2, glUniform2iv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(int, 3, glUniform3iv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(int, 4, glUniform4iv)

MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(unsigned, 1, glUniform1uiv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(unsigned, 2, glUniform2uiv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(unsigned, 3, glUniform3uiv)
MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(unsigned, 4, glUniform4uiv)

#define MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(rw, cn, fn)\
	template<> void MayaShaderProgram::SetUniformMatrix(MayaStringCR name, MayaMatrix<float, rw, cn> const& mat)\
	{ window->UseGraphicsContext(); s_BindShaderProgram(programid); int loc = FindUniformLocation(name); fn(loc, 1, false, &mat[0][0]); }

MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(2, 2, glUniformMatrix2fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(2, 3, glUniformMatrix2x3fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(2, 4, glUniformMatrix2x4fv)

MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(3, 2, glUniformMatrix3x2fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(3, 3, glUniformMatrix3fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(3, 4, glUniformMatrix3x4fv)

MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(4, 2, glUniformMatrix4x2fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(4, 3, glUniformMatrix4x3fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(4, 4, glUniformMatrix4fv)