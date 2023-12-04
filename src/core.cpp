#include <Maya/core.hpp>
#include <GLFW/glfw3.h>

void MayaInitLibrary()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void MayaTerminateLibrary()
{
	glfwTerminate();
}