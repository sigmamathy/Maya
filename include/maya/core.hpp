#pragma once

#if !defined(__cplusplus)
#error cpp is required
#endif

#if defined (_MSC_VER) // msvc compiler
#define MAYA_CXX_VERSION _MSVC_LANG
#elif defined(__GNUC__) || defined(__clang__) // gcc or clang
#define MAYA_CXX_VERSION __cplusplus
#else // otherwise
#error unknown compiler detected, only MSVC, GCC and CLANG are supported
#endif

#if MAYA_CXX_VERSION < 202002L // version lower than C++20
#error C++20 is the minimum version requried, try compile with /std:c++20 or -std=c++20
#endif

#ifndef MAYA_DEBUG // if not predefined
#if (defined (_MSC_VER) && defined (_DEBUG)) || ((defined (__GNUC__) || defined (__clang__)) && defined (__OPTIMIZE__))
#define MAYA_DEBUG 1
#define MAYA_DIF(...) if (__VA_ARGS__)
#else
#define MAYA_DEBUG 0
#define MAYA_DIF(...) if (0)
#endif
// msvc: _DEBUG
// gcc or clang: __OPTIMIZE__
#endif

#if defined (_WIN32) // windows
#define MAYA_PLATFORM_WINDOWS 1
#elif defined (__APPLE__) // macos
#define MAYA_PLATFORM_MACOS 1
#elif defined (__linux__) // linux
#define MAYA_PLATFORM_LINUX 1
#else // otherwise
#error unknown platform detected, only Windows, MacOS and Linux are supported
#endif

#include <string>
#include <string_view>
#include <cmath>
#include <array>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <type_traits>
#include <concepts>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <variant>

#define MAYA_TYPEDEF0(x, y) using x = y; using x##R = y&; using x##CR = y const&
#define MAYA_TYPEDEF1(x, y) template<class Ty> using x = y; template<class Ty> using x##R = y&; template<class Ty> using x##CR = y const&
#define MAYA_TYPEDEF2(x, ...) template<class Ty1, class Ty2> using x = __VA_ARGS__; template<class Ty1, class Ty2> using x##R = __VA_ARGS__&; template<class Ty1, class Ty2> using x##CR = __VA_ARGS__ const&
#define MAYA_TYPEDEFPTR(x) class x; MAYA_TYPEDEF0(x##Uptr, MayaUptr<x>); MAYA_TYPEDEF0(x##Sptr, MayaSptr<x>); MAYA_TYPEDEF0(x##Wptr, MayaWptr<x>)

MAYA_TYPEDEF0(MayaString,		std::string);

MAYA_TYPEDEF1(MayaUptr,			std::unique_ptr<Ty>);
MAYA_TYPEDEF1(MayaSptr,			std::shared_ptr<Ty>);
MAYA_TYPEDEF1(MayaWptr,			std::weak_ptr<Ty>);
MAYA_TYPEDEF1(MayaArrayList,	std::vector<Ty>);
MAYA_TYPEDEF1(MayaFunction,		std::function<Ty>);

MAYA_TYPEDEF2(MayaHashMap,		std::unordered_map<Ty1, Ty2>);

// Error types
#define MAYA_NO_ERROR							0x0
#define MAYA_BOUNDARY_ERROR						0x1
#define MAYA_DIVISION_BY_ZERO_ERROR				0x2
#define MAYA_MISSING_LIBRARY_ERROR				0x3
#define MAYA_MISSING_FILE_ERROR					0x4
#define MAYA_EMPTY_REFERENCE_ERROR				0x5
#define MAYA_SHADER_COMPILE_ERROR				0x6
#define MAYA_INVALID_GRAPHICS_CONTEXT_ERROR		0x7

struct MayaErrorStatus
{
	int ErrorCode;
	MayaString Details;
	double TimeSinceInit;
	inline operator bool() const { return ErrorCode; }
};

void MayaSetErrorCallback(MayaFunctionCR<void(MayaErrorStatus& stat)> callback);

MayaErrorStatus MayaPollError();

#define MAYA_SERR(code, det) MayaSendError(MayaErrorStatus{code, det, MayaGetCurrentTimeSinceInit()})

void MayaSendError(MayaErrorStatus stat);

struct MayaLibraryRAII {
	MayaLibraryRAII();
	~MayaLibraryRAII();
	MayaLibraryRAII(MayaLibraryRAII const&) = delete;
	MayaLibraryRAII& operator=(MayaLibraryRAII const&) = delete;
};

// Initialize Maya (external) libraries as needed.
void MayaInitLibrary(void);

// Terminate Maya (external) libraries as needed.
void MayaTerminateLibrary(void);

// Returns true if MayaInitLibrary is called.
bool MayaIsLibraryInitialized(void);

double MayaGetCurrentTimeSinceInit(void);

void tmp();