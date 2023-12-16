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
#else
#define MAYA_DEBUG 0
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

// Error information provided internally
struct MayaErrorInfo {
	int code;
	std::string details;
	bool is_debug;
};

// For internal usage (debug use)
#define MAYA_DERR(x, str) throw MayaErrorInfo { x, "Debug Error [" #x "] " + static_cast<std::string>(str), true }

// Error types
#define MAYA_BOUNDARY_ERROR				0x1
#define MAYA_DIVISION_BY_ZERO_ERROR		0x2
#define MAYA_MISSING_LIBRARY_ERROR		0x3
#define MAYA_MISSING_FILE_ERROR			0x4
#define MAYA_EMPTY_REFERENCE_ERROR		0x5

// Initialize Maya (external) libraries as needed.
// Most features required this initialization and is advised to call this function
// at the start of the application. Make sure to call MayaTerminateLibrary before
// the application exits. Calling this function multiple times could activate
// undefined behaviour.
void MayaInitLibrary(void);

// Terminate Maya (external) libraries as needed.
// This function should and only be called after MayaInitLibrary to free all
// allocated resources. Calling this function multiple times could activate
// undefined behaviour.
void MayaTerminateLibrary(void);

// Returns true if MayaLibraryInit is called.
// The general purpose of this function is to debug internally and provide
// useful debug information. Not a function that should be considered in an
// application.
bool MayaIsLibraryInitialized(void);