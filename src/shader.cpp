#include <maya/shader.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <fstream>

namespace maya
{

ShaderProgram::ShaderProgram(RenderContext& rc)
	: RenderResource(rc)
{
	programid = glCreateProgram();
	for (unsigned& i : shaderids) i = 0;
}

ShaderProgram::~ShaderProgram()
{
	CleanUp();
}

ShaderProgram::uptr ShaderProgram::MakeUnique(RenderContext& rc)
{
	return uptr(new ShaderProgram(rc));
}

ShaderProgram::sptr ShaderProgram::MakeShared(RenderContext& rc)
{
	return sptr(new ShaderProgram(rc));
}

void ShaderProgram::CompileShader(ShaderType type, char const* source)
{
	GLenum gltype = 0;
	switch (type) {
		case VertexShader: gltype = GL_VERTEX_SHADER; break;
		case FragmentShader: gltype = GL_FRAGMENT_SHADER; break;
		case GeometryShader: gltype = GL_GEOMETRY_SHADER; break;
	}

	auto& shader = shaderids[type];
	shader = glCreateShader(gltype);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		stl::string errmsg;
		errmsg.resize(512);
		glGetShaderInfoLog(shader, 512, NULL, &errmsg[0]);

		switch (type) {
			case VertexShader: errmsg = "Following error found in vertex shader\n" + errmsg; break;
			case FragmentShader: errmsg = "Following error found in fragment shader\n" + errmsg; break;
			case GeometryShader: errmsg = "Following error found in geometry shader\n" + errmsg; break;
		}

		Error::Send(Error::ShaderParse, "ShaderProgram::CompileShader(ShaderType, char const*): " + errmsg);
		glDeleteShader(shader);
		shader = 0;
		return;
	}

	glAttachShader(programid, shader);
}

void ShaderProgram::LinkProgram()
{
	MAYA_DIF(!shaderids[VertexShader] || !shaderids[FragmentShader])
	{
		Error::Send(Error::ShaderParse,
			"ShaderProgram::LinkProgram(): Vertex shader or fragment shader is absent.");
		return;
	}

	glLinkProgram(programid);

	GLint status;
	glGetProgramiv(programid, GL_LINK_STATUS, &status);

	if (!status)
	{
		stl::string errmsg;
		errmsg.resize(512);
		glGetProgramInfoLog(programid, 512, NULL, &errmsg[0]);
		Error::Send(Error::ShaderParse,
			"ShaderProgram::LinkProgram(): Following error found while linking shaders: " + errmsg);
	}

	for (int i = 0; i < shaderids.size(); i++) {
		if (shaderids[i])
			glDeleteShader(shaderids[i]);
	}
}

void ShaderProgram::CleanUp()
{
	if (programid)
	{
		RenderResource::CleanUp();
		glDeleteProgram(programid);
		programid = 0;
	}
}

void RenderContext::SetProgram(ShaderProgram* program)
{
	if (crntprogram == program) return;
	crntprogram = program;
	glUseProgram(program ? program->programid : 0);
}

int ShaderProgram::FindUniformLocation(stl::strview name)
{
	if (uniform_location_cache.count(name))
		return uniform_location_cache.at(name);
	stl::string str(name); // Ensure null terminated string
	int x = glGetUniformLocation(programid, str.c_str());
	uniform_location_cache[name] = x;
	if (x == -1) {
		Error::Send(Error::ShaderUniformNotFound,
			"ShaderProgram::FindUniformLocation(stl::strview): The required uniform \"" + str + "\" does not exists.");
	}
	return x;
}

#define MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(ty, sz, fn)\
	template<> void ShaderProgram::SetUniformVector(stl::strview name, Vector<ty, sz> const& vec)\
	{ rc.SetProgram(this); int loc = FindUniformLocation(name); fn(loc, 1, &vec[0]); }

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

//#define MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(sz, fn)\
//	template<> void MayaShaderProgram::SetUniformVector(MayaStringCR name, MayaVector<bool, sz> const& vec)\
//	{ window->UseGraphicsContext(); Maya_s_BindShaderProgram(window, programid); int loc = FindUniformLocation(name); fn(loc, 1, (int*)&vec[0]); }
//
//MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(1, glUniform1iv)
//MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(2, glUniform2iv)
//MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(3, glUniform3iv)
//MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(4, glUniform4iv)

#define MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(rw, cn, fn)\
	template<> void ShaderProgram::SetUniformMatrix(stl::strview name, Matrix<float, rw, cn> const& mat)\
	{ rc.SetProgram(this); int loc = FindUniformLocation(name); fn(loc, 1, false, &mat[0][0]); }

MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(2, 2, glUniformMatrix2fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(2, 3, glUniformMatrix2x3fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(2, 4, glUniformMatrix2x4fv)

MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(3, 2, glUniformMatrix3x2fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(3, 3, glUniformMatrix3fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(3, 4, glUniformMatrix3x4fv)

MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(4, 2, glUniformMatrix4x2fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(4, 3, glUniformMatrix4x3fv)
MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(4, 4, glUniformMatrix4fv)

}