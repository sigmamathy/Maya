#include <maya/shader.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <fstream>

namespace maya
{

ShaderProgram::ShaderProgram(RenderContext& rc)
{
	Init(rc);
}

ShaderProgram::~ShaderProgram()
{
	Free();
}

ShaderProgram::uptr ShaderProgram::MakeUnique(RenderContext& rc)
{
	return uptr(new ShaderProgram(rc));
}

ShaderProgram::sptr ShaderProgram::MakeShared(RenderContext& rc)
{
	return sptr(new ShaderProgram(rc));
}

void ShaderProgram::Init(RenderContext& rc)
{
	RenderResource::Init(rc);
	nativeid = glCreateProgram();
	for (auto& id : shaderids)
		id = 0;
	uniform_location_cache.reserve(20);
}

void ShaderProgram::Free()
{
	if (nativeid) {
		RenderResource::Free();
		glDeleteProgram(nativeid);
		nativeid = 0;
	}
}

void ShaderProgram::CompileShader(ShaderType type, char const* source)
{
	GLenum gltype = 0;
	switch (type) {
		case VERTEX: gltype = GL_VERTEX_SHADER; break;
		case FRAGMENT: gltype = GL_FRAGMENT_SHADER; break;
		case GEOMETRY: gltype = GL_GEOMETRY_SHADER; break;
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
			case VERTEX: errmsg = "Following error found in vertex shader\n" + errmsg; break;
			case FRAGMENT: errmsg = "Following error found in fragment shader\n" + errmsg; break;
			case GEOMETRY: errmsg = "Following error found in geometry shader\n" + errmsg; break;
		}

		auto& cm = *CoreManager::Instance();
		cm.MakeError(cm.SHADER_COMPILE_ERROR, errmsg);
		glDeleteShader(shader);
		shader = 0;
		return;
	}

	glAttachShader(nativeid, shader);
}

void ShaderProgram::LinkProgram()
{
#if MAYA_DEBUG
	if (!shaderids[VERTEX] || !shaderids[FRAGMENT])
	{
		auto& cm = *CoreManager::Instance();
		cm.MakeError(cm.SHADER_LINK_ERROR, "Vertex shader or fragment shader is absent.");
		return;
	}
#endif

	glLinkProgram(nativeid);

	GLint status;
	glGetProgramiv(nativeid, GL_LINK_STATUS, &status);

	if (!status)
	{
		stl::string errmsg;
		errmsg.resize(512);
		glGetProgramInfoLog(nativeid, 512, NULL, &errmsg[0]);
		auto& cm = *CoreManager::Instance();
		cm.MakeError(cm.SHADER_LINK_ERROR, "Following error found while linking shaders : " + errmsg);
	}

	for (int i = 0; i < shaderids.size(); i++) {
		if (shaderids[i])
			glDeleteShader(shaderids[i]);
	}
}

int ShaderProgram::FindUniformLocation(stl::strview name)
{
	if (uniform_location_cache.count(name))
		return uniform_location_cache.at(name);
	stl::string str(name); // Ensure null terminated string
	int x = glGetUniformLocation(nativeid, str.c_str());
	uniform_location_cache[name] = x;
#if MAYA_DEBUG
	if (x == -1)
		MAYA_DEBUG_LOG_WARNING("The required uniform \"" + str + "\" does not exists.");
#endif
	return x;
}

#define MAYA_DEFINE_UNIFORM_VECTOR_FUNCTION(ty, sz, fn)\
	template<> void ShaderProgram::SetUniformVector(stl::strview name, Vector<ty, sz> const& vec)\
	{ rc->SetProgram(this); int loc = FindUniformLocation(name); fn(loc, 1, &vec[0]); }

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

#define MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(sz, fn)\
	template<> void ShaderProgram::SetUniformVector(stl::strview name, Vector<bool, sz> const& vec)\
	{ rc->SetProgram(this); int loc = FindUniformLocation(name); Vector<int, sz> nv = vec; fn(loc, 1, &nv[0]); }

MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(1, glUniform1iv)
MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(2, glUniform2iv)
MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(3, glUniform3iv)
MAYA_DEFINE_UNIFORM_BOOL_VECTOR_FUNCTION(4, glUniform4iv)

#define MAYA_DEFINE_UNIFORM_MATRIX_FUNCTION(rw, cn, fn)\
	template<> void ShaderProgram::SetUniformMatrix(stl::strview name, Matrix<float, rw, cn> const& mat)\
	{ rc->SetProgram(this); int loc = FindUniformLocation(name); fn(loc, 1, false, &mat[0][0]); }

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