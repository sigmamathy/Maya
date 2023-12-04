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
	constexpr std::common_type_t<Ty, float> Norm() const;

	MAYA_VECTOR_COMMON
};

template<class Ty>
class MayaVector<Ty, 2>
{
public:
	Ty x, y;
	constexpr MayaVector(Ty x, Ty y);
	MAYA_VECTOR_COMMON
};

template<class Ty>
class MayaVector<Ty, 3>
{
public:
	Ty x, y, z;
	constexpr MayaVector(Ty x, Ty y, Ty z);
	MAYA_VECTOR_COMMON
};

template<class Ty>
class MayaVector<Ty, 4>
{
public:
	Ty x, y, z, w;
	constexpr MayaVector(Ty x, Ty y, Ty z, Ty w);
	MAYA_VECTOR_COMMON
};

template<class Ty1, class Ty2, int Dim>
constexpr auto operator+(MayaVector<Ty1, Dim> const& vec1, MayaVector<Ty2, Dim> const& vec2)
{
	MayaVector<decltype(vec1[0] + vec2[0]), Dim> res;
	for (int i = 0; i < Dim; i++)
		res[i] = vec1[i] + vec2[i];
	return res;
}

using MayaIvec2 = MayaVector<int, 2>;
using MayaIvec3 = MayaVector<int, 3>;
using MayaIvec4 = MayaVector<int, 4>;

using MayaUvec2 = MayaVector<unsigned, 2>;
using MayaUvec3 = MayaVector<unsigned, 3>;
using MayaUvec4 = MayaVector<unsigned, 4>;

using MayaFvec2 = MayaVector<float, 2>;
using MayaFvec3 = MayaVector<float, 3>;
using MayaFvec4 = MayaVector<float, 4>;

#include "./math.inl"