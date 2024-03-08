#pragma once

#include "./render.hpp"

namespace maya
{

enum ShaderType
{
	VertexShader,
	FragmentShader,
	GeometryShader
};

class ShaderProgram : public RenderResource
{
public:

	using uptr = stl::uptr<ShaderProgram>;
	using sptr = stl::sptr<ShaderProgram>;

	ShaderProgram(RenderContext& rc);

	~ShaderProgram();

	// No copy construct.
	ShaderProgram(ShaderProgram const&) = delete;
	ShaderProgram& operator=(ShaderProgram const&) = delete;

	static uptr MakeUnique(RenderContext& rc);

	static sptr MakeShared(RenderContext& rc);

	void CompileShader(ShaderType type, char const* source);

	void LinkProgram();

	void CleanUp() override;

	template<class Ty, class... Tys> requires (std::is_convertible_v<Tys, Ty> && ...)
	void SetUniform(stl::strview name, Tys... args);

	template<class Ty, int Sz>
	void SetUniformVector(stl::strview name, Vector<Ty, Sz> const& vec);

	template<int Rw, int Cn>
	void SetUniformMatrix(stl::strview name, Matrix<float, Rw, Cn> const& mat);

private:

	int programid;
	stl::array<unsigned, 3> shaderids;

	stl::hashmap<stl::strview, int> uniform_location_cache;
	friend class RenderContext;

	int FindUniformLocation(stl::strview name);
};

template<class Ty, class... Tys> requires (std::is_convertible_v<Tys, Ty> && ...)
void ShaderProgram::SetUniform(stl::strview name, Tys... args)
{
	Vector<Ty, sizeof...(Tys)> vec = { static_cast<Ty>(args)... };
	SetUniformVector(name, vec);
}

}