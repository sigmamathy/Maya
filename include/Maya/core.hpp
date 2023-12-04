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
#define MAYA_DEBUG (defined (_MSC_VER) && defined (_DEBUG))\
					|| ((defined (__GNUC__) || defined (__clang__)) && defined (__OPTIMIZE__))
// msvc: _DEBUG
// gcc or clang: __OPTIMIZE__
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