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
#define MAYA_DIF(...) if (__VA_ARGS__) [[unlikely]]
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
#include <cmath>
#include <array>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <type_traits>
#include <concepts>
#include <cmath>
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


// --------------- Error code constants --------------- //

#define MAYA_NO_ERROR							0x0
#define MAYA_BOUNDARY_ERROR						0x1
#define MAYA_DIVISION_BY_ZERO_ERROR				0x2
#define MAYA_MISSING_LIBRARY_ERROR				0x3
#define MAYA_MISSING_FILE_ERROR					0x4
#define MAYA_EMPTY_REFERENCE_ERROR				0x5
#define MAYA_SHADER_COMPILE_ERROR				0x6
#define MAYA_INCONSISTENTENCY_ERROR				0x7
#define MAYA_IMAGE_LOAD_ERROR					0x8
#define MAYA_SHADER_UNIFORM_NO_FOUND_ERROR		0x9
#define MAYA_INSTANCE_ERROR						0xA

/**
	@brief Struct of error data reported by Maya.

	Indicates the type of error in ErrorCode and
	briefly describes the cause of error in Details.
	Those error will not be thrown as exception, but instead
	reported using MayaSendError which can be retrieved later using
	MayaPollError or MayaSetErrorCallback.
*/
struct MayaError
{
	unsigned ErrorCode;
	MayaString Details;
	inline operator bool() const {
		return ErrorCode;
	}
};

/**
	@brief Set an error callback for Maya.

	Tells Maya which callback to use whenever MayaSendError is called.
	Also automatically poll all queued errors and send to this callback.
	By default, callback is set to nullptr and all errors will be store
	in the queue until MayaPollError is called.

	@param callback: The callback to be used, could be nullptr.
*/
void MayaSetErrorCallback(MayaFunctionCR<void(MayaError&)> callback);

/**
	@brief Returns the error from top of the error queue.

	Poll an error from the error queue. If no error exists, this returns
	error with ErrorCode = MAYA_NO_ERROR. One can use this advantage 
	in a while loop. For Instance:

	@code
	while (MayaError err = MayaPollError()) {
		// do stuff...
	}
	@endcode

	This function is absolutely useless if an error callback is set, since
	all errors are already handled and no error will be stored in the queue.
*/
MayaError MayaPollError();

/**
	@brief Report an error to Maya.

	Place the error into the back of error queue if callback is not specified,
	otherwise automatically send it to the callback provided by user.
*/
void MayaSendError(MayaError err);

/*
	@brief Log error to console via std::cerr
*/
void MayaLogErrorToStdCerr(MayaError& err);

// ---------- Bits field constants for specifying dependencies ---------- //

#define MAYA_LIBRARY_GLFW 0x1

/**
	@brief Load dependencies into memory.

	This instance should only be created once and sustain its lifetime
	throughout the program until main() returned. Some classes requires
	specific dependencies to be loaded to function. For details please
	refer to the above definition of constants.
*/
class MayaLibraryManager
{
public:

	/**
		@brief Initialize specific contents mentioned in bits.

		After desired features are loaded, MayaLibraryManager will free them
		automatically when it reaches its lifetime. Note that required libraries
		can be loaded later at any time, using the LoadDependencies function.

		@param bits: a bit field that specify what to load
	*/
	MayaLibraryManager(unsigned bits = 0);

	/**
		@brief Destroy and free all loaded libraries.
	*/
	~MayaLibraryManager();

	// No copy
	MayaLibraryManager(MayaLibraryManager const&) = delete;
	MayaLibraryManager& operator=(MayaLibraryManager const&) = delete;

	/**
		@brief Load more dependencies manually.

		Function exactly the same as the constructor.
		NEVER load libraries that has already been loaded,
		which may results undefined behaviour.

		@param bitfield: a bit field that specify what to load
	*/
	void LoadDependencies(unsigned bitfield);

	/**
		@brief Unload dependencies manually.

		Once a dependencies is unloaded, it will no longer
		be handled by the manager.
		NEVER unload libraries that have not been loaded,
		which may results undefined behaviour.

		@param bitfield: a bit field that specify what to unload
	*/
	void UnloadDependencies(unsigned bitfield);

	/**
		@brief Returns true if a dependencies is found to be loaded.

		@param bit: expects a single dependency
	*/
	bool FoundDependency(unsigned bit) const;

	/**
		@brief Returns the time since manager is created (in seconds)
	*/
	float GetTimeSince() const;

private:
	// libraries
	unsigned libraries;
};

/**
	@brief Returns a pointer to library manager if presents.
*/
MayaLibraryManager* MayaGetLibraryManager();