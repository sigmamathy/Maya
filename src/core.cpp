#include <maya/core.hpp>
#include <queue>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>

static std::queue<MayaError> s_error_queue;
static MayaFunction<void(MayaError& stat)> s_error_callback;

void MayaSetErrorCallback(MayaFunctionCR<void(MayaError& stat)> callback)
{
	s_error_callback = callback;
	if (callback)
		while (auto s = MayaPollError())
			s_error_callback(s);
}

MayaError MayaPollError()
{
	if (s_error_queue.empty())
		return MayaError{ .ErrorCode = MAYA_NO_ERROR };
	auto res = s_error_queue.front();
	s_error_queue.pop();
	return res;
}

void MayaSendError(MayaError stat)
{
	if (s_error_callback)
		s_error_callback(stat);
	else 
		s_error_queue.push(stat);
}

static MayaLibraryManager* s_library_pointer = 0;
static std::chrono::steady_clock::time_point s_library_start_tp;

MayaLibraryManager::MayaLibraryManager(unsigned bitfield)
	: libraries(bitfield)
{
	MAYA_DIF(s_library_pointer)
	{
		MayaSendError({
			MAYA_INSTANCE_ERROR,
			"MayaLibraryManager::MayaLibraryManager(): Multiple instances of MayaLibraryManager is detected."
		});
		return;
	}

	s_library_pointer = this;
	LoadDependencies(bitfield);
	s_library_start_tp = std::chrono::high_resolution_clock::now();
}

MayaLibraryManager::~MayaLibraryManager()
{
	UnloadDependencies(libraries);
	s_library_pointer = 0;
}

void MayaLibraryManager::LoadDependencies(unsigned bitfield)
{
	libraries |= bitfield;
	if (bitfield & MAYA_LIBRARY_GLFW) {
		glfwInit();
	}
}

void MayaLibraryManager::UnloadDependencies(unsigned bitfield)
{
	libraries &= ~bitfield;
	if (bitfield & MAYA_LIBRARY_GLFW) {
		glfwTerminate();
	}
}

bool MayaLibraryManager::FoundDependency(unsigned bit) const
{
	return libraries & bit;
}

float MayaLibraryManager::GetTimeSince() const
{
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = end - s_library_start_tp;
	return duration.count();
}

MayaLibraryManager* MayaGetLibraryManager()
{
	return s_library_pointer;
}