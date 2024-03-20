#include <maya/core.hpp>
#include <queue>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <portaudio.h>
#include <iostream>

namespace maya
{

struct s_Error {
	CoreManager::ErrorCode code;
	stl::string message;
};

static CoreManager* s_cm_pointer = 0;
static std::chrono::high_resolution_clock::time_point s_cm_start_tp;
static stl::fnptr<void(CoreManager::LogLevel, stl::string const&)> s_cm_logger;
static CoreManager::ErrorHandleMode s_cm_err_mode;
static std::queue<s_Error> s_cm_error_queue;

CoreManager::CoreManager()
{
#if MAYA_DEBUG
	if (s_cm_pointer)
	{
		s_cm_pointer->MakeError(INVALID_OPERATION_ERROR, "Core Manager is initialized twice.");
		return;
	}
#endif

	s_cm_pointer = this;
	s_cm_start_tp = std::chrono::high_resolution_clock::now();
	s_cm_logger = DefaultConsoleLogger;
	s_cm_err_mode = MAYA_DEBUG ? QUEUE_AND_LOG_ERROR : QUEUE_ERROR;
	glfwInit();
	Pa_Initialize();
}

CoreManager::~CoreManager()
{
	MAYA_DEBUG_LOG_INFO("CoreManager destructing...");
	glfwTerminate();
	Pa_Terminate();
	s_cm_pointer = 0;
}

CoreManager* CoreManager::Instance()
{
	return s_cm_pointer;
}

float CoreManager::GetTimeSince() const
{
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = end - s_cm_start_tp;
	return duration.count();
}

void CoreManager::Log(LogLevel level, stl::string const& content)
{
	if (s_cm_logger)
		s_cm_logger(level, content);
}

void CoreManager::SetLogger(CoreManager::Logger const& outputfn)
{
	s_cm_logger = outputfn;
}

CoreManager::Logger const& CoreManager::GetLogger() const
{
	return s_cm_logger;
}

void CoreManager::DefaultConsoleLogger(LogLevel level, stl::string const& content)
{
	std::cout << "[Maya:";
	switch (level) {
		case LOG_INFO: std::cout << "INFO]    | "; break;
		case LOG_WARNING: std::cout << "\033[93mWARNING\033[0m] | "; break;
		case LOG_ERROR: std::cout << "\033[91mERROR\033[0m]   | "; break;
	}
	std::cout << content << '\n';
}

void CoreManager::MakeError(ErrorCode code, stl::string const& msg)
{
	if (s_cm_err_mode % 2)
		Log(LOG_ERROR, "Code " + std::to_string(code) + " - Reason: " + msg);
	switch (s_cm_err_mode / 2) {
		case 1: s_cm_error_queue.push(s_Error{ code, msg }); break;
		case 2: throw code;
	}
}

void CoreManager::SetErrorHandleMode(ErrorHandleMode mode)
{
	s_cm_err_mode = mode;
}

}