
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
	template<class Ty> constexpr std::common_type_t<Ty, float> MayaVector<Ty, 2>::Norm() const { return std::sqrt(__VA_ARGS__); }

MAYA_TEMP_DEFINE_VECTOR_NORM(2, x * x + y * y)
MAYA_TEMP_DEFINE_VECTOR_NORM(3, x * x + y * y + z * z)
MAYA_TEMP_DEFINE_VECTOR_NORM(4, x * x + y * y + z * z + w * w)

#undef MAYA_TEMP_DEFINE_VECTOR_OPERATOR_BRACKETS
#undef MAYA_TEMP_DEFINE_VECTOR_NORM