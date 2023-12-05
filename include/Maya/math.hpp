#pragma once

#include "./core.hpp"

// General template for Vectors (Mathematics)
// Ty: must be an arithmetic type
// Dim: must be > 0 (2-4 is advised)
template<class Ty, int Dim>
class MayaVector
{
	// Common functions that applied to all variation of vectors
#define MAYA_VECTOR_COMMON	public:											\
																			\
	/* Value uninitialized construction */									\
	constexpr MayaVector(void) = default;									\
																			\
	/* Access modifier */													\
	constexpr Ty& operator[](int i);										\
																			\
	/* Constant access */													\
	constexpr Ty const& operator[](int i) const;							\
																			\
	/* Returns the norm (aka magnitude) of a vector */						\
	std::common_type_t<Ty, float> Norm() const;

	MAYA_VECTOR_COMMON
};

// 2D vector
template<class Ty>
class MayaVector<Ty, 2>
{
public:
	Ty x, y;
	constexpr MayaVector(Ty x, Ty y);
	MAYA_VECTOR_COMMON
};

// 3D vector
template<class Ty>
class MayaVector<Ty, 3>
{
public:
	Ty x, y, z;
	constexpr MayaVector(Ty x, Ty y, Ty z);
	MAYA_VECTOR_COMMON
};

// 4D vector
template<class Ty>
class MayaVector<Ty, 4>
{
public:
	Ty x, y, z, w;
	constexpr MayaVector(Ty x, Ty y, Ty z, Ty w);
	MAYA_VECTOR_COMMON
};

using MayaIvec2 = MayaVector<int, 2>;
using MayaIvec3 = MayaVector<int, 3>;
using MayaIvec4 = MayaVector<int, 4>;

using MayaUvec2 = MayaVector<unsigned, 2>;
using MayaUvec3 = MayaVector<unsigned, 3>;
using MayaUvec4 = MayaVector<unsigned, 4>;

using MayaFvec2 = MayaVector<float, 2>;
using MayaFvec3 = MayaVector<float, 3>;
using MayaFvec4 = MayaVector<float, 4>;

template<class Ty> constexpr MayaVector<Ty, 2>::MayaVector(Ty x, Ty y) : x(x), y(y) {}
template<class Ty> constexpr MayaVector<Ty, 3>::MayaVector(Ty x, Ty y, Ty z) : x(x), y(y), z(z) {}
template<class Ty> constexpr MayaVector<Ty, 4>::MayaVector(Ty x, Ty y, Ty z, Ty w) : x(x), y(y), z(z), w(w) {}

#if MAYA_DEBUG
#define MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(dim, ...)\
	template<class Ty> constexpr Ty& MayaVector<Ty, dim>::operator[](int i) { if (i < 0 || i > dim - 1) throw 1; __VA_ARGS__; }\
	template<class Ty> constexpr Ty const& MayaVector<Ty, dim>::operator[](int i) const { if (i < 0 || i > dim - 1) throw 1; __VA_ARGS__; }
#else
#define MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(dim, ...)\
	template<class Ty> constexpr Ty& MayaVector<Ty, dim>::operator[](int i) { __VA_ARGS__; }\
	template<class Ty> constexpr Ty const& MayaVector<Ty, dim>::operator[](int i) const { __VA_ARGS__; }
#endif

MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(2, return i ? y : x)
MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(3, return i ? (i == 2 ? z : y) : x)
MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(4, return i ? (i == 3 ? w : (i == 2 ? z : y)) : x)

#define MAYA_TEMP_DEFINE_VECTOR_NORM(dim, ...)\
	template<class Ty> std::common_type_t<Ty, float> MayaVector<Ty, 2>::Norm() const { return std::sqrt(__VA_ARGS__); }

MAYA_TEMP_DEFINE_VECTOR_NORM(2, x * x + y * y)
MAYA_TEMP_DEFINE_VECTOR_NORM(3, x * x + y * y + z * z)
MAYA_TEMP_DEFINE_VECTOR_NORM(4, x * x + y * y + z * z + w * w)

#undef MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS
#undef MAYA_TEMP_DEFINE_VECTOR_NORM

// Check equality of 2 vectors
template<class Ty1, class Ty2, int Dim>
constexpr auto operator==(MayaVector<Ty1, Dim> const& vec1, MayaVector<Ty2, Dim> const& vec2)
{
	for (int i = 0; i < Dim; i++)
		if (vec1[i] != vec2[i])
			return false;
	return true;
}

// Addition operation bewteen 2 vectors
template<class Ty1, class Ty2, int Dim>
constexpr auto operator+(MayaVector<Ty1, Dim> const& vec1, MayaVector<Ty2, Dim> const& vec2)
{
	MayaVector<decltype(vec1[0] + vec2[0]), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec1[i] + vec2[i];
	return res;
}

// Subtraction operation bewteen 2 vectors
template<class Ty1, class Ty2, int Dim>
constexpr auto operator-(MayaVector<Ty1, Dim> const& vec1, MayaVector<Ty2, Dim> const& vec2)
{
	MayaVector<decltype(vec1[0] - vec2[0]), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec1[i] - vec2[i];
	return res;
}

// Negate operation of vector
template<class Ty, int Dim>
constexpr auto operator-(MayaVector<Ty, Dim> const& vec)
{
	MayaVector<Ty, Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = -vec[i];
	return res;
}

// Scaler multiplication operation of vector
template<class Ty, class ScTy, int Dim> requires std::is_arithmetic_v<ScTy>
constexpr auto operator*(MayaVector<Ty, Dim> const& vec, ScTy scale)
{
	MayaVector<decltype(vec[0] * scale), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec[i] * scale;
	return res;
}

// Scaler multiplication operation of vector
template<class Ty, class ScTy, int Dim> requires std::is_arithmetic_v<ScTy>
constexpr auto operator*(ScTy scale, MayaVector<Ty, Dim> const& vec)
{
	return vec * scale;
}

// Scaler division operation of vector
template<class Ty, class ScTy, int Dim> requires std::is_arithmetic_v<ScTy>
constexpr auto operator/(MayaVector<Ty, Dim> const& vec, ScTy scale)
{
	MayaVector<decltype(vec[0] / scale), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec[i] / scale;
	return res;
}

// General template for Matrix (Mathematics)
// Ty: must be an arithmetic type
// Rw & Cn: must be > 0
template<class Ty, int Rw, int Cn>
class MayaMatrix
{
public:
	// Value uninitialized construction
	constexpr MayaMatrix() = default;

	// Identity matrix multiplied by value
	explicit constexpr MayaMatrix(Ty value);

	// Construct with Rw * Cn number of arguments
	template<class... Tys> requires (sizeof...(Tys) == Rw * Cn && (std::is_convertible_v<Tys, Ty> && ...))
		constexpr Matrix(Tys... args);

	// Access modifiers
	constexpr MayaVector<Ty, Cn>& operator[](int i);

	// Constant access
	constexpr MayaVector<Ty, Cn> const& operator[](int i) const;

private:
	std::array<MayaVector<Ty, Cn>, Rw> column_vectors;
};

template<class Ty, int Rw, int Cn>
constexpr MayaVector<Ty, Cn>& MayaMatrix<Ty, Rw, Cn>::operator[](int i)
{
	returns components[i];
}

template<class Ty, int Rw, int Cn>
constexpr MayaVector<Ty, Cn> const& MayaMatrix<Ty, Rw, Cn>::operator[](int i) const
{
	returns components[i];
}