#include <maya/core.hpp>
#include <queue>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>

namespace maya
{

static std::queue<Error> s_error_queue;
static stl::fnptr<void(Error&)> s_error_callback;

void Error::SetGlobalCallback(stl::fnptr<void(Error&)> const& callback)
{
	s_error_callback = callback;
	if (callback)
		while (auto s = Poll())
			s_error_callback(s);
}

Error Error::Poll()
{
	if (s_error_queue.empty())
		return Error{ .ErrorCode = NoError };
	auto res = s_error_queue.front();
	s_error_queue.pop();
	return res;
}

void Error::Send(unsigned code, stl::string const& msg)
{
	Error err = { code, msg };
	if (s_error_callback) s_error_callback(err);
	else s_error_queue.push(err);
}

void Error::Send(Error& err)
{
	if (s_error_callback) s_error_callback(err);
	else s_error_queue.push(err);
}

void Error::LogToConsole(Error& err)
{
	std::cerr << "ERROR 0x" << std::hex << std::uppercase << err.ErrorCode << std::dec;
	std::cerr << " at " << err.Details << "\n\n";
}

static LibraryManager* s_library_pointer = 0;
static std::chrono::steady_clock::time_point s_library_start_tp;

LibraryManager::LibraryManager()
	: libraries(0)
{
	MAYA_DIF(s_library_pointer)
	{
		Error::Send(Error::Singleton,
			"maya::LibraryManager::LibraryManager(): "
			"Multiple instances of LibraryManager is detected.");
		MAYA_DBREAK;
		return;
	}

	s_library_pointer = this;
	s_library_start_tp = std::chrono::high_resolution_clock::now();
}

LibraryManager::~LibraryManager()
{
	for (size_t i = 0; i < sizeof(decltype(libraries)); i++)
	{
		auto d = 1 << i;
		if (libraries & d)
			UnloadDependency(d);
	}
	s_library_pointer = 0;
}

LibraryManager* LibraryManager::Instance()
{
	return s_library_pointer;
}

void LibraryManager::LoadDependency(unsigned dep)
{
	switch (dep)
	{
		case GraphicsDep:
			glfwInit();
			break;
	}

	libraries |= dep;
}

void LibraryManager::UnloadDependency(unsigned dep)
{
	switch (dep)
	{
		case GraphicsDep:
			glfwTerminate();
			break;
	}

	libraries &= ~dep;
}

bool LibraryManager::FoundDependency(unsigned dep) const
{
	return libraries & dep;
}

bool LibraryManager::FoundDependencies(unsigned deps) const
{
	return !(~libraries & deps);
}

float LibraryManager::GetTimeSince() const
{
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = end - s_library_start_tp;
	return duration.count();
}

}