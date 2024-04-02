#pragma once

#include "./render.hpp"

namespace maya
{

// A complete shader pipeline.
class ShaderProgram : public RenderResource
{
public:

	using uptr = stl::uptr<ShaderProgram>;
	using sptr = stl::sptr<ShaderProgram>;

	// Uninitialized.
	ShaderProgram(void) = default;

	// Constructor.
	ShaderProgram(RenderContext& rc);

	// Cleanup resources.
	~ShaderProgram();

	// No copy construct.
	ShaderProgram(ShaderProgram const&) = delete;
	ShaderProgram& operator=(ShaderProgram const&) = delete;

	// Create and return a uptr.
	static uptr MakeUnique(RenderContext& rc);

	// Create and return a sptr.
	static sptr MakeShared(RenderContext& rc);

	// Initialize program.
	virtual void Init(RenderContext& rc) override;

	// Free program.
	virtual void Free() override;

	// Shader type in pipelines.
	enum ShaderType {
		VERTEX,
		FRAGMENT,
		GEOMETRY
	};

	// Complie a shader of a type.
	void CompileShader(ShaderType type, char const* source);

	// Link the shaders compiled.
	void LinkProgram();

	// Set a uniform.
	template<class Ty, class... Tys> requires (std::is_convertible_v<Tys, Ty> && ...)
	void SetUniform(stl::strview name, Tys... args) {
		Vector<Ty, sizeof...(Tys)> vec = { static_cast<Ty>(args)... };
		SetUniformVector(name, vec);
	}

	// Set a uniform vector.
	template<class Ty, int Sz>
	void SetUniformVector(stl::strview name, Vector<Ty, Sz> const& vec);

	// Set a uniform matrix.
	template<int Rw, int Cn>
	void SetUniformMatrix(stl::strview name, Matrix<float, Rw, Cn> const& mat);

private:

	stl::array<MAYA_STL uint32_t, 3> shaderids;
	stl::hashmap<stl::strview, int> uniform_location_cache;

	int FindUniformLocation(stl::strview name);
};

}