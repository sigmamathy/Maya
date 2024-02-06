#pragma once

#include "./core.hpp"

static constexpr double MayaConstPI = 3.141592653589793;

// General template for Vectors (Mathematics)
// Ty: must be an arithmetic type
// Dim: must be > 0 (2-4 is advised)
template<class Ty, int Dim>
class MayaVector
{
	// Common functions that applied to all variation of vectors
#define MAYA_VECTOR_COMMON(dim)	public:										\
																			\
	/* Value uninitialized construction */									\
	constexpr MayaVector(void) = default;									\
																			\
	/* Copy constructor */													\
	template<class Ty2>														\
	constexpr MayaVector(MayaVector<Ty2, dim> const&);						\
																			\
	/* Access modifier */													\
	constexpr Ty& operator[](int i);										\
																			\
	/* Constant access */													\
	constexpr Ty const& operator[](int i) const;							\
																			\
	/* Calculate the norm (aka magnitude) of a vector */					\
	std::common_type_t<Ty, float> Norm() const;

// ---------------------------- End --------------------------- //
	static_assert(Dim > 0);
	MAYA_VECTOR_COMMON(Dim)
	explicit constexpr MayaVector(auto value);

	// Construct with Dim number of arguments
	template<class... Tys> requires (Dim != 1 && sizeof...(Tys) == Dim && (std::is_convertible_v<Tys, Ty> && ...))
	constexpr MayaVector(Tys... args);

private:
	// std array implementation
	std::array<Ty, Dim> elems;
};

// 1D vector variation (x)
template<class Ty>
class MayaVector<Ty, 1>
{
public:
	Ty x;
	constexpr MayaVector(auto x);
	MAYA_VECTOR_COMMON(1)
};

// 2D vector variation (x, y)
template<class Ty>
class MayaVector<Ty, 2>
{
public:
	Ty x, y;
	constexpr MayaVector(auto x, auto y);
	explicit constexpr MayaVector(auto value);
	MAYA_VECTOR_COMMON(2)
};

// 3D vector variation (x, y, z)
template<class Ty>
class MayaVector<Ty, 3>
{
public:
	Ty x, y, z;
	constexpr MayaVector(auto x, auto y, auto z);
	explicit constexpr MayaVector(auto value);
	MAYA_VECTOR_COMMON(3)
};

// 4D vector variation (x, y, z, w)
template<class Ty>
class MayaVector<Ty, 4>
{
public:
	Ty x, y, z, w;
	constexpr MayaVector(auto x, auto y, auto z, auto w);
	explicit constexpr MayaVector(auto value);
	MAYA_VECTOR_COMMON(4)
};

#undef MAYA_VECTOR_COMMON

// -------------------- Typedefs ---------------------- //

using MayaIvec2 = MayaVector<int, 2>;
using MayaIvec3 = MayaVector<int, 3>;
using MayaIvec4 = MayaVector<int, 4>;

using MayaUvec2 = MayaVector<unsigned, 2>;
using MayaUvec3 = MayaVector<unsigned, 3>;
using MayaUvec4 = MayaVector<unsigned, 4>;

using MayaFvec2 = MayaVector<float, 2>;
using MayaFvec3 = MayaVector<float, 3>;
using MayaFvec4 = MayaVector<float, 4>;

// --------------------------------- Implementation ------------------------------ //

template<class Ty, int Dim>
constexpr MayaVector<Ty, Dim>::MayaVector(auto value)
{
	for (int i = 0; i < Dim; i++)
		elems[i] = static_cast<Ty>(value);
}

template<class Ty, int Dim> template<class... Tys>
	requires (Dim != 1 && sizeof...(Tys) == Dim && (std::is_convertible_v<Tys, Ty> && ...))
constexpr MayaVector<Ty, Dim>::MayaVector(Tys... args)
	: elems{ static_cast<Ty>(args)... }
{
}

template<class Ty, int Dim> template<class Ty2>
constexpr MayaVector<Ty, Dim>::MayaVector(MayaVector<Ty2, Dim> const& vec)
{
	for (int i = 0; i < Dim; i++)
		elems[i] = static_cast<Ty>(vec[i]);
}

template<class Ty, int Dim>
constexpr Ty& MayaVector<Ty, Dim>::operator[](int i)
{
	MAYA_DIF(i < 0 || i > Dim - 1)
		MAYA_SERR(MAYA_BOUNDARY_ERROR,
			"MayaVector::operator[](int): Array index out of bounds.");
	return elems[i];
}

template<class Ty, int Dim>
constexpr Ty const& MayaVector<Ty, Dim>::operator[](int i) const
{
	MAYA_DIF(i < 0 || i > Dim - 1)
		MAYA_SERR(MAYA_BOUNDARY_ERROR,
			"MayaVector::operator[](int): Array index out of bounds.");
	return elems[i];
}

template<class Ty, int Dim>
std::common_type_t<Ty, float> MayaVector<Ty, Dim>::Norm() const
{
	Ty sum = 0;
	for (int i = 0; i < Dim; i++)
		sum += elems[i] * elems[i];
	return std::sqrt(static_cast<std::common_type_t<Ty, float>>(sum));
}

template<class Ty> constexpr MayaVector<Ty, 1>::MayaVector(auto x) 
	: x(static_cast<Ty>(x)) {}
template<class Ty> constexpr MayaVector<Ty, 2>::MayaVector(auto x, auto y) 
	: x(static_cast<Ty>(x)), y(static_cast<Ty>(y)) {}
template<class Ty> constexpr MayaVector<Ty, 3>::MayaVector(auto x, auto y, auto z) 
	: x(static_cast<Ty>(x)), y(static_cast<Ty>(y)), z(static_cast<Ty>(z)) {}
template<class Ty> constexpr MayaVector<Ty, 4>::MayaVector(auto x, auto y, auto z, auto w) 
	: x(static_cast<Ty>(x)), y(static_cast<Ty>(y)), z(static_cast<Ty>(z)), w(static_cast<Ty>(w)) {}

template<class Ty> constexpr MayaVector<Ty, 2>::MayaVector(auto value) 
	: x(static_cast<Ty>(value)), y(static_cast<Ty>(value)) {}
template<class Ty> constexpr MayaVector<Ty, 3>::MayaVector(auto value) 
	: x(static_cast<Ty>(value)), y(static_cast<Ty>(value)), z(static_cast<Ty>(value)) {}
template<class Ty> constexpr MayaVector<Ty, 4>::MayaVector(auto value) 
	: x(static_cast<Ty>(value)), y(static_cast<Ty>(value)), z(static_cast<Ty>(value)), w(static_cast<Ty>(value)) {}

template<class Ty> template<class Ty2> constexpr MayaVector<Ty, 1>::MayaVector(MayaVector<Ty2, 1> const& vec)
	: x(static_cast<Ty>(vec.x))
{
}

template<class Ty> template<class Ty2> constexpr MayaVector<Ty, 2>::MayaVector(MayaVector<Ty2, 2> const& vec)
	: x(static_cast<Ty>(vec.x)), y(static_cast<Ty>(vec.y))
{
}

template<class Ty> template<class Ty2> constexpr MayaVector<Ty, 3>::MayaVector(MayaVector<Ty2, 3> const& vec)
	: x(static_cast<Ty>(vec.x)), y(static_cast<Ty>(vec.y)), z(static_cast<Ty>(vec.z))
{
}

template<class Ty> template<class Ty2> constexpr MayaVector<Ty, 4>::MayaVector(MayaVector<Ty2, 4> const& vec)
	: x(static_cast<Ty>(vec.x)), y(static_cast<Ty>(vec.y)), z(static_cast<Ty>(vec.z)), w(static_cast<Ty>(vec.w))
{
}

#define MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(dim, ...)\
	template<class Ty> constexpr Ty& MayaVector<Ty, dim>::operator[](int i) {\
			MAYA_DIF (i < 0 || i > dim - 1) MAYA_SERR(MAYA_BOUNDARY_ERROR, "MayaVector::operator[](int): Array index out of bounds."); __VA_ARGS__; }\
	template<class Ty> constexpr Ty const& MayaVector<Ty, dim>::operator[](int i) const {\
			MAYA_DIF (i < 0 || i > dim - 1) MAYA_SERR(MAYA_BOUNDARY_ERROR, "MayaVector::operator[](int): Array index out of bounds."); __VA_ARGS__; }

MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(1, return x)
MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(2, return i ? y : x)
MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(3, return i ? (i == 2 ? z : y) : x)
MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS(4, return i ? (i == 3 ? w : (i == 2 ? z : y)) : x)

#define MAYA_TEMP_DEFINE_VECTOR_NORM(dim, ...)\
	template<class Ty> std::common_type_t<Ty, float> MayaVector<Ty, dim>::Norm() const { return std::sqrt(__VA_ARGS__); }

MAYA_TEMP_DEFINE_VECTOR_NORM(1, x * x)
MAYA_TEMP_DEFINE_VECTOR_NORM(2, x * x + y * y)
MAYA_TEMP_DEFINE_VECTOR_NORM(3, x * x + y * y + z * z)
MAYA_TEMP_DEFINE_VECTOR_NORM(4, x * x + y * y + z * z + w * w)

#undef MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS
#undef MAYA_TEMP_DEFINE_VECTOR_NORM

// Compare equality of 2 vectors
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
	MayaVector<decltype(-vec[0]), Dim> res;
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

// Concatenate two vectors into a larger vector
template<class Ty1, int Dim1, class Ty2, int Dim2>
constexpr auto MayaConcat(MayaVector<Ty1, Dim1> const& vec1, MayaVector<Ty2, Dim2> const& vec2)
{
	MayaVector<std::common_type_t<Ty1, Ty2>, Dim1 + Dim2> res;
	int n = 0;
	for (int i = 0; i < Dim1; i++) res[n++] = vec1[i];
	for (int i = 0; i < Dim2; i++) res[n++] = vec2[i];
	return res;
}

// Dot product operation
template<class Ty1, class Ty2, int Dim>
constexpr auto MayaDot(MayaVector<Ty1, Dim> const& vec1, MayaVector<Ty2, Dim> const& vec2)
{
	decltype(vec1[0] * vec2[0]) res = 0;
	for (int i = 0; i < Dim; i++)
		res += vec1[i] * vec2[i];
	return res;
}

// Cross product operation
template<class Ty1, class Ty2>
constexpr auto MayaCross(MayaVector<Ty1, 3> const& vec1, MayaVector<Ty2, 3> const& vec2)
{
	return MayaVector<decltype(vec1[0] * vec2[0]), 3>
	{
		vec1[1] * vec2[2] - vec1[2] * vec2[1],
			vec1[2] * vec2[0] - vec1[0] * vec2[2],
			vec1[0] * vec2[1] - vec1[1] * vec2[0]
	};
}

// Cross product operation on 2D (special case)
template<class Ty1, class Ty2>
constexpr auto MayaCross2D(MayaVector<Ty1, 2> const& vec1, MayaVector<Ty2, 2> const& vec2)
{
	return vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

// Vector normalization
template<class Ty, int Dim>
constexpr auto MayaNormalize(MayaVector<Ty, Dim> const& vec)
{
	using common_type = std::common_type_t<Ty, float>;
	Ty sum = 0;
	for (int i = 0; i < Dim; i++)
		sum += vec[i] * vec[i];
	MAYA_DIF(!sum)
		MAYA_SERR(MAYA_DIVISION_BY_ZERO_ERROR,
			"MayaNormalize(MayaVector const&): The required vector has norm equal to zero.");
	auto invsqrt = 1.0f / std::sqrt(static_cast<common_type>(sum));
	MayaVector<common_type, Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec[i] * invsqrt;
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
	template<class... Tys>
		requires (sizeof...(Tys) == Rw * Cn && (std::is_convertible_v<Tys, Ty> && ...) && (std::is_arithmetic_v<Tys> && ...))
	constexpr MayaMatrix(Tys... args);

	// Construct with Rw * Cn number of arguments
	template<class... Tys> requires (sizeof...(Tys) == Cn)
		constexpr MayaMatrix(MayaVector<Tys, Rw> const&... columns);

	// Copy constructor
	template<class Ty2>
	constexpr MayaMatrix(MayaMatrix<Ty2, Rw, Cn> const& mat);

	// Access modifiers
	constexpr MayaVector<Ty, Rw>& operator[](int i);

	// Constant access
	constexpr MayaVector<Ty, Rw> const& operator[](int i) const;

private:
	// array of column vectors
	std::array<MayaVector<Ty, Rw>, Cn> column_vectors;
};

// -------------------- Typedefs ---------------------- //

using MayaImat2x2 = MayaMatrix<int, 2, 2>;
using MayaImat2x3 = MayaMatrix<int, 2, 3>;
using MayaImat2x4 = MayaMatrix<int, 2, 4>;
using MayaImat3x2 = MayaMatrix<int, 3, 2>;
using MayaImat3x3 = MayaMatrix<int, 3, 3>;
using MayaImat3x4 = MayaMatrix<int, 3, 4>;
using MayaImat4x2 = MayaMatrix<int, 4, 2>;
using MayaImat4x3 = MayaMatrix<int, 4, 3>;
using MayaImat4x4 = MayaMatrix<int, 4, 4>;

using MayaUmat2x2 = MayaMatrix<unsigned, 2, 2>;
using MayaUmat2x3 = MayaMatrix<unsigned, 2, 3>;
using MayaUmat2x4 = MayaMatrix<unsigned, 2, 4>;
using MayaUmat3x2 = MayaMatrix<unsigned, 3, 2>;
using MayaUmat3x3 = MayaMatrix<unsigned, 3, 3>;
using MayaUmat3x4 = MayaMatrix<unsigned, 3, 4>;
using MayaUmat4x2 = MayaMatrix<unsigned, 4, 2>;
using MayaUmat4x3 = MayaMatrix<unsigned, 4, 3>;
using MayaUmat4x4 = MayaMatrix<unsigned, 4, 4>;

using MayaFmat2x2 = MayaMatrix<float, 2, 2>;
using MayaFmat2x3 = MayaMatrix<float, 2, 3>;
using MayaFmat2x4 = MayaMatrix<float, 2, 4>;
using MayaFmat3x2 = MayaMatrix<float, 3, 2>;
using MayaFmat3x3 = MayaMatrix<float, 3, 3>;
using MayaFmat3x4 = MayaMatrix<float, 3, 4>;
using MayaFmat4x2 = MayaMatrix<float, 4, 2>;
using MayaFmat4x3 = MayaMatrix<float, 4, 3>;
using MayaFmat4x4 = MayaMatrix<float, 4, 4>;

using MayaImat2 = MayaImat2x2;
using MayaImat3 = MayaImat3x3;
using MayaImat4 = MayaImat4x4;

using MayaUmat2 = MayaUmat2x2;
using MayaUmat3 = MayaUmat3x3;
using MayaUmat4 = MayaUmat4x4;

using MayaFmat2 = MayaFmat2x2;
using MayaFmat3 = MayaFmat3x3;
using MayaFmat4 = MayaFmat4x4;

// --------------------------------- Implementation ------------------------------ //

template<class Ty, int Rw, int Cn>
constexpr MayaMatrix<Ty, Rw, Cn>::MayaMatrix(Ty value)
{
	for (int i = 0; i < Cn; i++) {
		for (int j = 0; j < Rw; j++) {
			if (i == j) column_vectors[i][j] = value;
			else column_vectors[i][j] = 0;
		}
	}
}

template<class Ty, int Rw, int Cn> template<class... Tys>
	requires (sizeof...(Tys) == Rw * Cn && (std::is_convertible_v<Tys, Ty> && ...) && (std::is_arithmetic_v<Tys> && ...))
constexpr MayaMatrix<Ty, Rw, Cn>::MayaMatrix(Tys... args)
{
	std::array<Ty, Rw* Cn> const largs = { static_cast<Ty>(args)... };
	int n = 0;
	for (int j = 0; j < Rw; j++)
		for (int i = 0; i < Cn; i++)
			column_vectors[i][j] = largs[n++];
}

template<class Ty, int Rw, int Cn> template<class... Tys> requires (sizeof...(Tys) == Cn)
constexpr MayaMatrix<Ty, Rw, Cn>::MayaMatrix(MayaVector<Tys, Rw> const&... columns)
	: column_vectors{ static_cast<MayaVector<Ty, Rw>>(columns)... }
{
}

template<class Ty, int Rw, int Cn> template<class Ty2>
constexpr MayaMatrix<Ty, Rw, Cn>::MayaMatrix(MayaMatrix<Ty2, Rw, Cn> const& mat)
{
	for (int i = 0; i < Cn; i++)
		column_vectors[i] = mat.column_vectors[i];
}

template<class Ty, int Rw, int Cn>
constexpr MayaVector<Ty, Rw>& MayaMatrix<Ty, Rw, Cn>::operator[](int i)
{
	MAYA_DIF(i < 0 || i > Cn - 1)
		MAYA_SERR(MAYA_BOUNDARY_ERROR,
			"MayaMatrix::operator[](int): Array index out of bounds.");
	return column_vectors[i];
}

template<class Ty, int Rw, int Cn>
constexpr MayaVector<Ty, Rw> const& MayaMatrix<Ty, Rw, Cn>::operator[](int i) const
{
	MAYA_DIF(i < 0 || i > Cn - 1)
		MAYA_SERR(MAYA_BOUNDARY_ERROR,
			"MayaMatrix::operator[](int): Array index out of bounds.");
	return column_vectors[i];
}

// Compare equality of 2 matrices
template<class Ty1, class Ty2, int Rw, int Cn>
constexpr auto operator==(MayaMatrix<Ty1, Rw, Cn> const& mat1, MayaMatrix<Ty2, Rw, Cn> const& mat2)
{
	for (int i = 0; i < Cn; i++)
		for (int j = 0; j < Rw; j++)
			if (mat1[i][j] != mat2[i][j])
				return false;
	return true;
}

// Addition operation between 2 matrices
template<class Ty1, class Ty2, int Rw, int Cn>
constexpr auto operator+(MayaMatrix<Ty1, Rw, Cn> const& mat1, MayaMatrix<Ty2, Rw, Cn> const& mat2)
{
	MayaMatrix<decltype(mat1[0][0] + mat2[0][0]), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		for (int j = 0; j < Rw; j++)
			res[i][j] = mat1[i][j] + mat2[i][j];
	return res;
}

// Subtraction operation between 2 matrices
template<class Ty1, class Ty2, int Rw, int Cn>
constexpr auto operator-(MayaMatrix<Ty1, Rw, Cn> const& mat1, MayaMatrix<Ty2, Rw, Cn> const& mat2)
{
	MayaMatrix<decltype(mat1[0][0] - mat2[0][0]), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		for (int j = 0; j < Rw; j++)
			res[i][j] = mat1[i][j] - mat2[i][j];
	return res;
}

// Negate operation of matrices
template<class Ty, int Rw, int Cn>
constexpr auto operator-(MayaMatrix<Ty, Rw, Cn> const& mat)
{
	MayaMatrix<decltype(-mat[0][0]), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		for (int j = 0; j < Rw; j++)
			res[i][j] = -mat[i][j];
	return res;
}

// Scalar mulitplication operation of matrices
template<class Ty1, class ScTy, int Rw, int Cn>
constexpr auto operator*(MayaMatrix<Ty1, Rw, Cn> const& mat, ScTy scale)
{
	MayaMatrix<decltype(mat[0][0] * scale), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		for (int j = 0; j < Rw; j++)
			res[i][j] = mat[i][j] * scale;
	return res;
}

// Scalar mulitplication operation of matrices
template<class Ty1, class ScTy, int Rw, int Cn>
constexpr auto operator*(ScTy scale, MayaMatrix<Ty1, Rw, Cn> const& mat)
{
	return mat * scale;
}

// Scalar division operation of matrices
template<class Ty1, class ScTy, int Rw, int Cn>
constexpr auto operator/(MayaMatrix<Ty1, Rw, Cn> const& mat, ScTy scale)
{
	MayaMatrix<decltype(mat[0][0] / scale), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		for (int j = 0; j < Rw; j++)
			res[i][j] = mat[i][j] / scale;
	return res;
}

// Matrix multiplication operation
template<class Ty1, class Ty2, int Rw1, int Rw2Cn1, int Cn2>
constexpr auto operator*(MayaMatrix<Ty1, Rw1, Rw2Cn1> const& mat1, MayaMatrix<Ty2, Rw2Cn1, Cn2> const& mat2)
{
	MayaMatrix<decltype(mat1[0][0] * mat2[0][0]), Rw1, Cn2> res(0);
	for (int i = 0; i < Cn2; i++)
		for (int j = 0; j < Rw1; j++)
			for (int k = 0; k < Rw2Cn1; k++)
				res[i][j] += mat1[k][j] * mat2[i][k];
	return res;
}

// Matrix multiplication operation (with vector outcome)
template<class Ty1, class Ty2, int Rw1, int Dim>
constexpr auto operator*(MayaMatrix<Ty1, Rw1, Dim> const& mat, MayaVector<Ty2, Dim> const& vec)
{
	MayaVector<decltype(mat[0][0] * vec[0]), Rw1> res(0);
	for (int j = 0; j < Rw1; j++)
		for (int i = 0; i < Dim; i++)
			res[j] += mat[i][j] * vec[i];
	return res;
}

// Matrix transpose operation
template<class Ty, int Rw, int Cn>
constexpr auto MayaTranspose(MayaMatrix<Ty, Rw, Cn> const& mat)
{
	MayaMatrix<Ty, Cn, Rw> res;
	for (int j = 0; j < Rw; j++)
		for (int i = 0; i < Cn; i++)
			res[j][i] = mat[i][j];
	return res;
}