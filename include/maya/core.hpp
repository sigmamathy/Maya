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
#if defined(_MSC_VER)
#define MAYA_DBREAK __debugbreak()
#else
#define MAYA_DBREAK __builtin_trap()
#endif
#else
#define MAYA_DEBUG 0
#define MAYA_DIF(...) if (0)
#define MAYA_DBREAK 
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

#define MAYA_BENCHMARK(...) {\
	float _maya_tstart = ::maya::CoreManager::Instance()->GetTimeSince();\
	__VA_ARGS__; std::cout << ::maya::CoreManager::Instance()->GetTimeSince() - _maya_tstart << '\n'; }

#include <string>
#include <string_view>
#include <cmath>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <type_traits>
#include <concepts>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

namespace maya
{

// Types and Functions from the C++ STL library
namespace stl
{

using string = ::std::string;

using strview = ::std::string_view;

using thread = ::std::thread;

using mutex = ::std::mutex;

template<class Fty>
using fnptr = ::std::function<Fty>;

template<class Ty>
using uptr = ::std::unique_ptr<Ty>;

template<class Ty>
using sptr = ::std::shared_ptr<Ty>;

template<class Ty, size_t Sz>
using array = ::std::array<Ty, Sz>;

template<class Ty>
using list = ::std::vector<Ty>;

template<class Ty>
using atomic = ::std::atomic<Ty>;

template<class KTy, class VTy>
using hashmap = ::std::unordered_map<KTy, VTy>;

template<class KTy>
using uset = ::std::unordered_set<KTy>;

}

template<class Ty>
struct Buffer
{
	Ty* Data = 0;
	unsigned Size = 0;
};

template<class Ty>
using ConstBuffer = Buffer<Ty const>;

// Provides useful error information.
struct Error
{
	// Standard error codes
	enum: unsigned
	{
		NoError = 0,
		OutOfBounds,
		DivisionByZero,
		MissingDependencies,
		FileNotFound,
		UnsupportedFileFormat,
		MissingReference,
		ShaderParse,
		Inconsistence,
		ImageLoad,
		ShaderUniformNotFound,
		Singleton,
	};

	// One of the error code defined above
	unsigned ErrorCode = NoError;

	// Error details, usually contains the function name too.
	stl::string Details;

	// Returns true if error presents
	inline operator bool() const noexcept {
		return ErrorCode;
	}

	// Use std::cerr to log error to console
	static void LogToConsole(Error& err);

	// Set a global callback for Maya to handle the errors.
	// Errors stored in the queue will be passed immediately to the callback.
	// By default callback is nullptr.
	static void SetGlobalHandle(stl::fnptr<void(Error&)> const& callback);

	// Poll the top error that is stored in the queue.
	// If none, one with NoError will be returned.
	static Error Poll();

	// Send an error to the callback if one is presented,
	// or push the error to the queue that can be retrieved later.
	static void Send(unsigned code, stl::string const& msg);

	// Send an error to the callback if one is presented,
	// or push the error to the queue that can be retrieved later.
	static void Send(Error& err);
};

// List of dependencies available.
enum Dependency : unsigned
{
	GraphicsDep		= 0x1,
	AudioDep		= 0x2,
};

// Served as the loader for dependencies.
class CoreManager
{
public:

	// One only instance of this class can be presented at a time.
	CoreManager();

	// Unload all dependencies if any.
	~CoreManager();

	// No copy construct.
	CoreManager(CoreManager const&) = delete;
	CoreManager& operator=(CoreManager const&) = delete;

	// Returns the instance of the class, or nullptr if none.
	static CoreManager* Instance();

	// Load a single dependency
	void LoadDependency(Dependency dep);

	// Equivalent to LoadDependency
	CoreManager& operator<<(Dependency dep);

	// Unload a single dependency
	void UnloadDependency(Dependency dep);

	// Returns true if the dependency is presented.
	bool FoundDependency(Dependency dep) const;

	// Returns true if the dependencies is presented.
	bool FoundDependencies(unsigned deps) const;

#define MAYA_FOUND_DEPS(deps) (CoreManager::Instance() && CoreManager::Instance()->FoundDependencies(deps))

	// Returns the time since the instance is created, in seconds.
	float GetTimeSince() const;

private:
	// bit fields
	unsigned dependencies;
};

}