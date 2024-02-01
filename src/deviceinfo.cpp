#include <maya/deviceinfo.hpp>
#include <GLFW/glfw3.h>


void MayaGetDeviceInfo(MayaMonitorsInfo* info)
{
	MAYA_DIF (!MayaIsLibraryInitialized())
		MAYA_SERR(MAYA_MISSING_LIBRARY_ERROR,
			"MayaGetDeviceInfo(MayaMonitorsInfo&): Required library is not initialized.");

	static std::vector<MayaMonitorsInfo::MonitorData> monitors_info_cache;
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
				MayaIvec2(vid->width, vid->height), ms[i]
			);
		}
		first_called = true;
	}

	info->Monitors = monitors_info_cache.data();
	info->Count = static_cast<int>(monitors_info_cache.size());
}