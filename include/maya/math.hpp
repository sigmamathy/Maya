#pragma once

#include "./core.hpp"

namespace maya
{

// -------------------------- List of math constants -------------------------- //

constexpr double Pi					= 3.141592653589793;
constexpr double Tau				= 6.283185307179586;
constexpr double Euler				= 2.718281828459045;
constexpr double Sqrt2				= 1.414213562373095;
constexpr double Sqrt3				= 1.732050807568877;
constexpr double Sqrt5				= 2.236067977499789;
constexpr double Ln2				= 0.693147180559945;
constexpr double GoldenRatio		= 1.618033988749894;
constexpr double Degree				= 0.017453292529943;

// Mathematical vectors
template<class Ty, int Dim>
class Vector
{
public:

	// Value uninitialized.
	constexpr Vector() noexcept = default;

	// Copy constructor.
	template<class Ty2>
	constexpr Vector(Vector<Ty2, Dim> const& vec) noexcept
	{
		for (int i = 0; i < Dim; i++)
			elems[i] = static_cast<Ty>(vec[i]);
	}

	// Value fill initialization.
	explicit constexpr Vector(auto value) noexcept
	{
		for (int i = 0; i < Dim; i++)
			elems[i] = static_cast<Ty>(value);
	}

	// Construct with Dim number of arguments.
	template<class... Tys>
		requires (sizeof...(Tys) == Dim && (std::is_convertible_v<Tys, Ty> && ...))
	constexpr Vector(Tys... args) noexcept
		: elems{ static_cast<Ty>(args)... }
	{
	}

	// Access elements.
	constexpr Ty& operator[](int i)
	{
		return elems[i];
	}

	// Access elements const.
	constexpr Ty const& operator[](int i) const
	{
		return elems[i];
	}

	// Calculate the magnitude.
	auto Norm() const
	{
		using common = std::common_type_t<Ty, float>;
		common sum = 0;
		for (int i = 0; i < Dim; i++)
			sum += static_cast<common>(elems[i] * elems[i]);
		return std::sqrt(sum);
	}

	template<class Ty2>
	Vector<Ty, Dim>& operator+=(Vector<Ty2, Dim> const& vec)
	{
		for (int i = 0; i < Dim; i++)
			elems[i] += static_cast<Ty>(vec[i]);
		return *this;
	}

	template<class Ty2>
	Vector<Ty, Dim>& operator*=(Vector<Ty2, Dim> const& vec)
	{
		for (int i = 0; i < Dim; i++)
			elems[i] -= static_cast<Ty>(vec[i]);
		return *this;
	}

	Vector<Ty, Dim>& operator*=(auto scale)
	{
		for (int i = 0; i < Dim; i++)
			elems[i] *= scale;
		return *this;
	}

private:
	// array implementation
	stl::array<Ty, Dim> elems;
};

// 1D vector variation (x)
template<class Ty>
class Vector<Ty, 1>
{
public:

	Ty x;

	constexpr Vector() noexcept = default;

	constexpr Vector(auto x) noexcept
	{
		this->x = static_cast<Ty>(x);
	}

	template<class Ty2> constexpr Vector(Vector<Ty2, 1> const& vec) noexcept
	{
		x = static_cast<Ty>(vec.x);
	}

	constexpr Ty& operator[](int i)
	{
		return x;
	}

	constexpr Ty const& operator[](int i) const
	{
		return x;
	}

	auto Norm() const
	{
		return x;
	}

	template<class Ty2>
	Vector<Ty, 1>& operator+=(Vector<Ty2, 1> const& vec)
	{
		x += static_cast<Ty>(vec.x);
		return *this;
	}

	template<class Ty2>
	Vector<Ty, 1>& operator-=(Vector<Ty2, 1> const& vec)
	{
		x -= static_cast<Ty>(vec.x);
		return *this;
	}

	Vector<Ty, 1>& operator*=(auto scale)
	{
		x *= static_cast<Ty>(scale);
		return *this;
	}
};

// 2D vector variation (x, y)
template<class Ty>
class Vector<Ty, 2>
{
public:

	Ty x, y;

	constexpr Vector() noexcept = default;

	explicit constexpr Vector(auto value) noexcept
	{
		x = y = static_cast<Ty>(value);
	}

	constexpr Vector(auto x, auto y) noexcept
	{
		this->x = static_cast<Ty>(x);
		this->y = static_cast<Ty>(y);
	}

	template<class Ty2> constexpr Vector(Vector<Ty2, 2> const& vec) noexcept
	{
		x = static_cast<Ty>(vec.x);
		y = static_cast<Ty>(vec.y);
	}

	constexpr Ty& operator[](int i)
	{
		return i ? y : x;
	}

	constexpr Ty const& operator[](int i) const
	{
		return i ? y : x;
	}

	auto Norm() const
	{
		return std::sqrt(x * x + y * y);
	}

	template<class Ty2>
	Vector<Ty, 2>& operator+=(Vector<Ty2, 2> const& vec)
	{
		x += static_cast<Ty>(vec.x);
		y += static_cast<Ty>(vec.y);
		return *this;
	}

	template<class Ty2>
	Vector<Ty, 2>& operator-=(Vector<Ty2, 2> const& vec)
	{
		x -= static_cast<Ty>(vec.x);
		y -= static_cast<Ty>(vec.y);
		return *this;
	}

	Vector<Ty, 2>& operator*=(auto scale)
	{
		x *= static_cast<Ty>(scale);
		y *= static_cast<Ty>(scale);
		return *this;
	}
};

// 3D vector variation (x, y, z)
template<class Ty>
class Vector<Ty, 3>
{
public:

	Ty x, y, z;

	constexpr Vector() noexcept = default;

	explicit constexpr Vector(auto value) noexcept
	{
		x = y = z = static_cast<Ty>(value);
	}

	constexpr Vector(auto x, auto y, auto z) noexcept
	{
		this->x = static_cast<Ty>(x);
		this->y = static_cast<Ty>(y);
		this->z = static_cast<Ty>(z);
	}

	template<class Ty2> constexpr Vector(Vector<Ty2, 3> const& vec) noexcept
	{
		x = static_cast<Ty>(vec.x);
		y = static_cast<Ty>(vec.y);
		z = static_cast<Ty>(vec.z);
	}

	constexpr Ty& operator[](int i)
	{
		return i == 2 ? z : (i ? y : x);
	}

	constexpr Ty const& operator[](int i) const
	{
		return i == 2 ? z : (i ? y : x);
	}

	auto Norm() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	template<class Ty2>
	Vector<Ty, 3>& operator+=(Vector<Ty2, 3> const& vec)
	{
		x += static_cast<Ty>(vec.x);
		y += static_cast<Ty>(vec.y);
		z += static_cast<Ty>(vec.z);
		return *this;
	}

	template<class Ty2>
	Vector<Ty, 3>& operator-=(Vector<Ty2, 3> const& vec)
	{
		x -= static_cast<Ty>(vec.x);
		y -= static_cast<Ty>(vec.y);
		z -= static_cast<Ty>(vec.z);
		return *this;
	}

	Vector<Ty, 3>& operator*=(auto scale)
	{
		x *= static_cast<Ty>(scale);
		y *= static_cast<Ty>(scale);
		z *= static_cast<Ty>(scale);
		return *this;
	}
};

// 4D vector variation (x, y, z, w)
template<class Ty>
class Vector<Ty, 4>
{
public:

	Ty x, y, z, w;

	constexpr Vector() noexcept = default;

	explicit constexpr Vector(auto value) noexcept
	{
		x = y = z = w = static_cast<Ty>(value);
	}

	constexpr Vector(auto x, auto y, auto z, auto w) noexcept
	{
		this->x = static_cast<Ty>(x);
		this->y = static_cast<Ty>(y);
		this->z = static_cast<Ty>(z);
		this->w = static_cast<Ty>(w);
	}

	template<class Ty2> constexpr Vector(Vector<Ty2, 4> const& vec) noexcept
	{
		x = static_cast<Ty>(vec.x);
		y = static_cast<Ty>(vec.y);
		z = static_cast<Ty>(vec.z);
		w = static_cast<Ty>(vec.w);
	}

	constexpr Ty& operator[](int i)
	{
		return i == 3 ? w : (i == 2 ? z : (i ? y : x));
	}

	constexpr Ty const& operator[](int i) const
	{
		return i == 3 ? w : (i == 2 ? z : (i ? y : x));
	}

	auto Norm() const
	{
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	template<class Ty2>
	Vector<Ty, 4>& operator+=(Vector<Ty2, 4> const& vec)
	{
		x += static_cast<Ty>(vec.x);
		y += static_cast<Ty>(vec.y);
		z += static_cast<Ty>(vec.z);
		w += static_cast<Ty>(vec.w);
		return *this;
	}

	template<class Ty2>
	Vector<Ty, 4>& operator-=(Vector<Ty2, 4> const& vec)
	{
		x -= static_cast<Ty>(vec.x);
		y -= static_cast<Ty>(vec.y);
		z -= static_cast<Ty>(vec.z);
		w -= static_cast<Ty>(vec.w);
		return *this;
	}

	Vector<Ty, 4>& operator*=(auto scale)
	{
		x *= static_cast<Ty>(scale);
		y *= static_cast<Ty>(scale);
		z *= static_cast<Ty>(scale);
		w *= static_cast<Ty>(scale);
		return *this;
	}
};

// -------------------- Typedefs ---------------------- //

using Ivec2 = Vector<int, 2>;
using Ivec3 = Vector<int, 3>;
using Ivec4 = Vector<int, 4>;

using Uvec2 = Vector<unsigned, 2>;
using Uvec3 = Vector<unsigned, 3>;
using Uvec4 = Vector<unsigned, 4>;

using Fvec2 = Vector<float, 2>;
using Fvec3 = Vector<float, 3>;
using Fvec4 = Vector<float, 4>;

// ------------------------------------ Functions ------------------------------------ //

// Returns true if 2 vector are equal
template<class Ty1, class Ty2, int Dim>
constexpr bool operator==(Vector<Ty1, Dim> const& vec1, Vector<Ty2, Dim> const& vec2)
{
	for (int i = 0; i < Dim; i++)
		if (vec1[i] != vec2[i])
			return false;
	return true;
}

// Addition operation bewteen 2 vectors
template<class Ty1, class Ty2, int Dim>
constexpr auto operator+(Vector<Ty1, Dim> const& vec1, Vector<Ty2, Dim> const& vec2)
{
	Vector<decltype(vec1[0] + vec2[0]), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec1[i] + vec2[i];
	return res;
}

// Subtraction operation bewteen 2 vectors
template<class Ty1, class Ty2, int Dim>
constexpr auto operator-(Vector<Ty1, Dim> const& vec1, Vector<Ty2, Dim> const& vec2)
{
	Vector<decltype(vec1[0] - vec2[0]), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec1[i] - vec2[i];
	return res;
}

// Negate operation of vector
template<class Ty, int Dim>
constexpr auto operator-(Vector<Ty, Dim> const& vec)
{
	Vector<decltype(-vec[0]), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = -vec[i];
	return res;
}

// Scaler multiplication operation of vector
template<class Ty, class ScTy, int Dim> requires std::is_arithmetic_v<ScTy>
constexpr auto operator*(Vector<Ty, Dim> const& vec, ScTy scale)
{
	Vector<decltype(vec[0] * scale), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec[i] * scale;
	return res;
}

// Scaler multiplication operation of vector
template<class Ty, class ScTy, int Dim> requires std::is_arithmetic_v<ScTy>
constexpr auto operator*(ScTy scale, Vector<Ty, Dim> const& vec)
{
	return vec * scale;
}

// Scaler division operation of vector
template<class Ty, class ScTy, int Dim> requires std::is_arithmetic_v<ScTy>
constexpr auto operator/(Vector<Ty, Dim> const& vec, ScTy scale)
{
	Vector<decltype(vec[0] / scale), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec[i] / scale;
	return res;
}

// Concatenate two vectors into a larger vector
template<class Ty1, int Dim1, class Ty2, int Dim2>
constexpr auto operator&(Vector<Ty1, Dim1> const& vec1, Vector<Ty2, Dim2> const& vec2)
{
	Vector<std::common_type_t<Ty1, Ty2>, Dim1 + Dim2> res;
	int n = 0;
	for (int i = 0; i < Dim1; i++) res[n++] = vec1[i];
	for (int i = 0; i < Dim2; i++) res[n++] = vec2[i];
	return res;
}

// Concatenate a vector and a number into a larger vector
template<class Ty, int Dim, class ScTy> requires std::is_arithmetic_v<ScTy>
constexpr auto operator&(Vector<Ty, Dim> const& vec, ScTy num)
{
	Vector<std::common_type_t<Ty, ScTy>, Dim + 1> res;
	for (int i = 0; i < Dim; i++) res[i] = vec[i];
	res[Dim] = static_cast<Ty>(num);
	return res;
}

// Concatenate a vector and a number into a larger vector
template<class Ty, int Dim, class ScTy> requires std::is_arithmetic_v<ScTy>
constexpr auto operator&(ScTy num, Vector<Ty, Dim> const& vec)
{
	Vector<std::common_type_t<Ty, ScTy>, Dim + 1> res;
	res[0] = static_cast<Ty>(num);
	for (int i = 0; i < Dim; i++) res[i + 1] = vec[i];
	return res;
}

// Dot product operation
template<class Ty1, class Ty2, int Dim>
constexpr auto Dot(Vector<Ty1, Dim> const& vec1, Vector<Ty2, Dim> const& vec2)
{
	decltype(vec1[0] * vec2[0]) res = 0;
	for (int i = 0; i < Dim; i++)
		res += vec1[i] * vec2[i];
	return res;
}

// Cross product operation
template<class Ty1, class Ty2>
constexpr auto Cross(Vector<Ty1, 3> const& vec1, Vector<Ty2, 3> const& vec2)
{
	return Vector<decltype(vec1[0] * vec2[0]), 3>
	{
		vec1[1] * vec2[2] - vec1[2] * vec2[1],
		vec1[2] * vec2[0] - vec1[0] * vec2[2],
		vec1[0] * vec2[1] - vec1[1] * vec2[0]
	};
}

// Cross product operation on 2D (special case)
template<class Ty1, class Ty2>
constexpr auto Cross2D(Vector<Ty1, 2> const& vec1, Vector<Ty2, 2> const& vec2)
{
	return vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

// Compute a normalized vector
template<class Ty, int Dim>
constexpr auto Normalize(Vector<Ty, Dim> const& vec)
{
	using common_type = std::common_type_t<Ty, float>;
	Ty sum = 0;
	for (int i = 0; i < Dim; i++)
		sum += vec[i] * vec[i];
	MAYA_DIF(!sum)
	{
		Error::Send(Error::DivisionByZero,
			"MayaNormalize(Vector const&): The required vector has norm equal to zero.");
		return Vector<common_type, Dim>(0);
	}
	auto invsqrt = 1.0f / std::sqrt(static_cast<common_type>(sum));
	Vector<common_type, Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec[i] * invsqrt;
	return res;
}

// Mathematical matrix
template<class Ty, int Rw, int Cn>
class Matrix
{
public:
	// Value uninitialized construction
	constexpr Matrix() noexcept = default;

	// Identity matrix multiplied by value
	explicit constexpr Matrix(Ty value) noexcept
	{
		std::fill(columns.begin(), columns.end(), Vector<Ty, Rw>(0));
		constexpr int max = Rw > Cn ? Rw : Cn;
		for (int i = 0; i < max; i++)
			columns[i][i] = value;
	}

	// Construct with Rw * Cn number of arguments
	template<class... Tys> requires (sizeof...(Tys) == Cn)
	constexpr Matrix(Vector<Tys, Rw> const&... columns) noexcept
		: columns { static_cast<Vector<Ty, Rw>>(columns)... }
	{
	}

	// Copy constructor
	template<class Ty2>
	constexpr Matrix(Matrix<Ty2, Rw, Cn> const& mat) noexcept
	{
		for (int i = 0; i < Cn; i++)
			columns[i] = mat.columns[i];
	}

	// Access modifiers
	constexpr Vector<Ty, Rw>& operator[](int i)
	{
		return columns[i];
	}

	// Constant access
	constexpr Vector<Ty, Rw> const& operator[](int i) const
	{
		return columns[i];
	}

	template<class Ty2>
	Matrix<Ty, Rw, Cn>& operator+=(Matrix<Ty2, Rw, Cn> const& mat)
	{
		for (int i = 0; i < Cn; i++)
			columns[i] += mat[i];
		return *this;
	}

	template<class Ty2>
	Matrix<Ty, Rw, Cn>& operator-=(Matrix<Ty2, Rw, Cn> const& mat)
	{
		for (int i = 0; i < Cn; i++)
			columns[i] -= mat[i];
		return *this;
	}

	Matrix<Ty, Rw, Cn>& operator*=(auto scale)
	{
		for (int i = 0; i < Cn; i++)
			columns[i] *= scale;
		return *this;
	}

private:
	// array of column vectors
	stl::array<Vector<Ty, Rw>, Cn> columns;
};

// -------------------- Typedefs ---------------------- //

using Imat2x2 = Matrix<int, 2, 2>;
using Imat2x3 = Matrix<int, 2, 3>;
using Imat2x4 = Matrix<int, 2, 4>;
using Imat3x2 = Matrix<int, 3, 2>;
using Imat3x3 = Matrix<int, 3, 3>;
using Imat3x4 = Matrix<int, 3, 4>;
using Imat4x2 = Matrix<int, 4, 2>;
using Imat4x3 = Matrix<int, 4, 3>;
using Imat4x4 = Matrix<int, 4, 4>;

using Umat2x2 = Matrix<unsigned, 2, 2>;
using Umat2x3 = Matrix<unsigned, 2, 3>;
using Umat2x4 = Matrix<unsigned, 2, 4>;
using Umat3x2 = Matrix<unsigned, 3, 2>;
using Umat3x3 = Matrix<unsigned, 3, 3>;
using Umat3x4 = Matrix<unsigned, 3, 4>;
using Umat4x2 = Matrix<unsigned, 4, 2>;
using Umat4x3 = Matrix<unsigned, 4, 3>;
using Umat4x4 = Matrix<unsigned, 4, 4>;

using Fmat2x2 = Matrix<float, 2, 2>;
using Fmat2x3 = Matrix<float, 2, 3>;
using Fmat2x4 = Matrix<float, 2, 4>;
using Fmat3x2 = Matrix<float, 3, 2>;
using Fmat3x3 = Matrix<float, 3, 3>;
using Fmat3x4 = Matrix<float, 3, 4>;
using Fmat4x2 = Matrix<float, 4, 2>;
using Fmat4x3 = Matrix<float, 4, 3>;
using Fmat4x4 = Matrix<float, 4, 4>;

using Imat2 = Imat2x2;
using Imat3 = Imat3x3;
using Imat4 = Imat4x4;

using Umat2 = Umat2x2;
using Umat3 = Umat3x3;
using Umat4 = Umat4x4;

using Fmat2 = Fmat2x2;
using Fmat3 = Fmat3x3;
using Fmat4 = Fmat4x4;

// ------------------------------------ Functions ------------------------------------ //

// Compare equality of 2 matrices
template<class Ty1, class Ty2, int Rw, int Cn>
constexpr auto operator==(Matrix<Ty1, Rw, Cn> const& mat1, Matrix<Ty2, Rw, Cn> const& mat2)
{
	for (int i = 0; i < Cn; i++)
		if (mat1[i] != mat2[i])
			return false;
	return true;
}

// Addition operation between 2 matrices
template<class Ty1, class Ty2, int Rw, int Cn>
constexpr auto operator+(Matrix<Ty1, Rw, Cn> const& mat1, Matrix<Ty2, Rw, Cn> const& mat2)
{
	Matrix<decltype(mat1[0][0] + mat2[0][0]), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		res[i] = mat1[i] + mat2[i];
	return res;
}

// Subtraction operation between 2 matrices
template<class Ty1, class Ty2, int Rw, int Cn>
constexpr auto operator-(Matrix<Ty1, Rw, Cn> const& mat1, Matrix<Ty2, Rw, Cn> const& mat2)
{
	Matrix<decltype(mat1[0][0] - mat2[0][0]), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		res[i] = mat1[i] - mat2[i];
	return res;
}

// Negate operation of matrices
template<class Ty, int Rw, int Cn>
constexpr auto operator-(Matrix<Ty, Rw, Cn> const& mat)
{
	Matrix<decltype(-mat[0][0]), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		res[i] = -mat[i];
	return res;
}

// Scalar mulitplication operation of matrices
template<class Ty1, class ScTy, int Rw, int Cn>
constexpr auto operator*(Matrix<Ty1, Rw, Cn> const& mat, ScTy scale)
{
	Matrix<decltype(mat[0][0] * scale), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		res[i] *= scale;
	return res;
}

// Scalar mulitplication operation of matrices
template<class Ty1, class ScTy, int Rw, int Cn>
constexpr auto operator*(ScTy scale, Matrix<Ty1, Rw, Cn> const& mat)
{
	return mat * scale;
}

// Scalar division operation of matrices
template<class Ty1, class ScTy, int Rw, int Cn>
constexpr auto operator/(Matrix<Ty1, Rw, Cn> const& mat, ScTy scale)
{
	Matrix<decltype(mat[0][0] / scale), Rw, Cn> res;
	for (int i = 0; i < Cn; i++)
		res[i] = mat[i] / scale;
	return res;
}

// Matrix multiplication operation
template<class Ty1, class Ty2, int Rw1, int Rw2Cn1, int Cn2>
constexpr auto operator*(Matrix<Ty1, Rw1, Rw2Cn1> const& mat1, Matrix<Ty2, Rw2Cn1, Cn2> const& mat2)
{
	Matrix<decltype(mat1[0][0] * mat2[0][0]), Rw1, Cn2> res(0);
	for (int i = 0; i < Cn2; i++)
		for (int j = 0; j < Rw1; j++)
			for (int k = 0; k < Rw2Cn1; k++)
				res[i][j] += mat1[k][j] * mat2[i][k];
	return res;
}

// Matrix multiplication operation (with vector outcome)
template<class Ty1, class Ty2, int Rw1, int Dim>
constexpr auto operator*(Matrix<Ty1, Rw1, Dim> const& mat, Vector<Ty2, Dim> const& vec)
{
	Vector<decltype(mat[0][0] * vec[0]), Rw1> res(0);
	for (int j = 0; j < Rw1; j++)
		for (int i = 0; i < Dim; i++)
			res[j] += mat[i][j] * vec[i];
	return res;
}

// Matrix transpose operation
template<class Ty, int Rw, int Cn>
constexpr auto Transpose(Matrix<Ty, Rw, Cn> const& mat)
{
	Matrix<Ty, Cn, Rw> res;
	for (int j = 0; j < Rw; j++)
		for (int i = 0; i < Cn; i++)
			res[j][i] = mat[i][j];
	return res;
}

}