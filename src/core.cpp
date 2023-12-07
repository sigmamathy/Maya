#include <Maya/core.hpp>
#include <GLFW/glfw3.h>

static bool is_library_initialized = false;

void MayaInitLibrary(void)
{
	is_library_initialized = true;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void MayaTerminateLibrary(void)
{
	is_library_initialized = false;
	glfwTerminate();
}

bool MayaIsLibraryInitialized(void)
{
	return is_library_initialized;
}