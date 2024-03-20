#include <maya/deviceinfo.hpp>
#include <GLFW/glfw3.h>

namespace maya
{

MonitorsInfo MonitorsInfo::Retrieve()
{
	MAYA_DIF(!CoreManager::Instance() || !CoreManager::Instance()->FoundDependency(GraphicsDep))
	{
		Error::Send(Error::MissingDependencies,
			"MayaGetDeviceInfo(MayaMonitorsInfo&): Graphics dependency not found.");
		return {};
	}

	MonitorsInfo result;
	static std::vector<MonitorsInfo::MonitorData> monitors_info_cache;
	static bool first_called = false;

	if (!first_called) {
		int count;
		GLFWmonitor** ms = glfwGetMonitors(&count);
		monitors_info_cache.reserve(count);
		for (int i = 0; i < count; i++)
		{
			auto* vid = glfwGetVideoMode(ms[i]);
			monitors_info_cache.emplace_back(
				i, glfwGetMonitorName(ms[i]),
				Ivec2(vid->width, vid->height), ms[i]
			);
		}
		first_called = true;
	}

	result.Monitors = monitors_info_cache.data();
	result.Count = static_cast<int>(monitors_info_cache.size());
	return result;
}

char const* GetClipBoardString()
{
	MAYA_DIF(!CoreManager::Instance() || !CoreManager::Instance()->FoundDependency(GraphicsDep))
	{
		Error::Send(Error::MissingDependencies,
			"MayaGetDeviceInfo(MayaMonitorsInfo&): GLFW is not initialized.");
		return nullptr;
	}

	return glfwGetClipboardString(0);
}

}