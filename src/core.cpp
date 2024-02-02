#include <maya/core.hpp>
#include <queue>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static std::queue<MayaErrorStatus> s_error_queue;
static MayaFunction<void(MayaErrorStatus& stat)> s_error_callback;

void MayaSetErrorCallback(MayaFunctionCR<void(MayaErrorStatus& stat)> callback)
{
	s_error_callback = callback;
	if (callback)
		while (auto s = MayaPollError())
			s_error_callback(s);
}

MayaErrorStatus MayaPollError()
{
	if (s_error_queue.empty())
		return MayaErrorStatus{ .ErrorCode = MAYA_NO_ERROR };
	auto res = s_error_queue.front();
	s_error_queue.pop();
	return res;
}

void MayaSendError(MayaErrorStatus stat)
{
	if (s_error_callback)
		s_error_callback(stat);
	else 
		s_error_queue.push(stat);
}

static bool s_library_initialized = false;

static void s_InitLibrary(void)
{
	MAYA_DIF(s_library_initialized) {
		MAYA_SERR(MAYA_SINGLETON_ERROR,
			"MayaLibrarySingleton::MayaLibrarySingleton(): Multiple instances of MayaLibrarySingleton is detected.");
		return;
	}
	s_library_initialized = true;
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void s_TerminateLibrary(void)
{
	s_library_initialized = false;
	glfwTerminate();
}

MayaLibrarySingleton::MayaLibrarySingleton()
{
	s_InitLibrary();
}

MayaLibrarySingleton::~MayaLibrarySingleton()
{
	s_TerminateLibrary();
}

bool MayaIsLibraryFound(void)
{
	return s_library_initialized;
}

double MayaGetCurrentTimeSinceInit(void)
{
	return glfwGetTime();
}

void tmp()
{
	std::cout << glGetError();
}