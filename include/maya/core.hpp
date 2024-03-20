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

#define MAYA_BENCHMARK(...) {\
	float _maya_tstart = ::maya::CoreManager::Instance()->GetTimeSince();\
	__VA_ARGS__; std::cout << ::maya::CoreManager::Instance()->GetTimeSince() - _maya_tstart << '\n'; }
#define MAYA_STL ::std::
#define MAYA_LIKELY [[likely]]
#define MAYA_UNLIKELY [[unlikely]]

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
	using string			= MAYA_STL string;
	using strview			= MAYA_STL string_view;

	template<class Ty> using fnptr		= MAYA_STL function<Ty>;
	template<class Ty> using uptr		= MAYA_STL unique_ptr<Ty>;
	template<class Ty> using sptr		= MAYA_STL shared_ptr<Ty>;
	template<class Ty> using list		= MAYA_STL vector<Ty>;
	template<class Ty> using atomic		= MAYA_STL atomic<Ty>;
	template<class Ty> using hashset	= MAYA_STL unordered_set<Ty>;

	template<class Ty, MAYA_STL size_t Sz> using array = MAYA_STL array<Ty, Sz>;
	template<class Ty1, class Ty2> using hashmap = MAYA_STL unordered_map<Ty1, Ty2>;
}

// Contains information about data pointer and size.
template<class Ty> struct Buffer {
	Ty* Data = 0;
	unsigned Size = 0;
};

// Constant buffer data and size.
template<class Ty> using ConstBuffer = Buffer<Ty const>;

// Served as the core of Maya.
// Require instantiation before any other usage of Maya.
class CoreManager
{
public:

	// One only instance of this class can be presented at a time.
	CoreManager();

	// Free necessary resources.
	~CoreManager();

	// No copy construct.
	CoreManager(CoreManager const&) = delete;
	CoreManager& operator=(CoreManager const&) = delete;

	// Returns the instance of the class, or nullptr if none.
	static CoreManager* Instance();

	// Returns the time since the instance is created, in seconds.
	float GetTimeSince() const;

	// Indicates the level of attention required in logging.
	enum LogLevel { LOG_INFO, LOG_WARNING, LOG_ERROR };

	// Logger typedef.
	using Logger = stl::fnptr<void(LogLevel, stl::string const&)>;

	// Default logger used by CoreManager.
	static void DefaultConsoleLogger(LogLevel level, stl::string const& content);

	// Set a custom logger, or nullptr if no output is desired.
	void SetLogger(Logger const& outputfn);

	// Get the current logger.
	Logger const& GetLogger() const;

	// Log information to logger.
	void Log(LogLevel level, stl::string const& content);

#if MAYA_DEBUG
#define MAYA_DEBUG_LOG_INFO(...) ::maya::CoreManager::Instance()->Log(::maya::CoreManager::LOG_INFO, __VA_ARGS__)
#define MAYA_DEBUG_LOG_WARNING(...) ::maya::CoreManager::Instance()->Log(::maya::CoreManager::LOG_WARNING, __VA_ARGS__)
#define MAYA_DEBUG_LOG_ERROR(...) ::maya::CoreManager::Instance()->Log(::maya::CoreManager::LOG_WARNING, __VA_ARGS__)
#else // !MAYA_DEBUG
#define MAYA_DEBUG_LOG_INFO(...)
#define MAYA_DEBUG_LOG_WARNING(...)
#define MAYA_DEBUG_LOG_ERROR(...)
#endif

	enum ErrorCode {
		NO_ERROR,
		INVALID_OPERATION_ERROR,
		OUT_OF_BOUNDS_ERROR,
		MATH_ERROR,
		FILE_NOT_FOUND_ERROR,
		FILE_FORMAT_ERROR,
		VERTEX_BUFFER_ERROR,
		SHADER_COMPILE_ERROR,
		SHADER_LINK_ERROR,
	};

	void MakeError(ErrorCode code, stl::string const& msg);

#define MAYA_MAKE_ERROR(code, ...) ::maya::CoreManager::Instance()->MakeError(::maya::CoreManager::code, __VA_ARGS__);

	enum ErrorHandleMode {
		IGNORE_ERROR,
		IGNORE_AND_LOG_ERROR,
		QUEUE_ERROR,
		QUEUE_AND_LOG_ERROR,
		THROW_ERROR,
		THROW_AND_LOG_ERROR
	};

	void SetErrorHandleMode(ErrorHandleMode mode);

};

}