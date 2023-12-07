#include <Maya/deviceinfo.hpp>

#if MAYA_PLATFORM_WINDOWS
#include <Windows.h>
#elif MAYA_PLATFORM_MACOS
#include <sys/types.h>
#include <sys/sysctl.h>
#elif MAYA_PLATFORM_LINUX
#include <sys/sysinfo.h>
#endif

#include <GLFW/glfw3.h>
#include <intrin.h>

static std::vector<MayaMonitorsInfo::components> monitors_info_cache;

void MayaGetDeviceInfo(MayaMonitorsInfo& info)
{
#if MAYA_DEBUG
	if (!MayaIsLibraryInitialized())
		MAYA_ERROR(MAYA_MISSING_LIBRARY_ERROR,
			"MayaGetDeviceInfo(MayaMonitorsInfo&): Required library is not initialized.");
#endif
	static bool first_call = false;
	if (!first_call) {
		int count;
		GLFWmonitor** ms = glfwGetMonitors(&count);
		monitors_info_cache.reserve(count);
		for (int i = 0; i < count; i++)
		{
			auto* vid = glfwGetVideoMode(ms[i]);
			monitors_info_cache.emplace_back (
				i, glfwGetMonitorName(ms[i]),
				MayaIvec2(vid->width, vid->height), ms[i]
			);
		}
		first_call = true;
	}
	info.monitors = &monitors_info_cache;
	info.count = static_cast<int>(monitors_info_cache.size());
}

void MayaGetDeviceInfo(MayaCPUInfo& info)
{
	int cpuInfo[4];

	char vendor[13];
	// Execute CPUID with leaf 0
	__cpuid(cpuInfo, 0);
	// Copy the vendor string from EBX, EDX, and ECX registers
	std::memcpy(vendor, &cpuInfo[1], sizeof(int));
	std::memcpy(vendor + 4, &cpuInfo[3], sizeof(int));
	std::memcpy(vendor + 8, &cpuInfo[2], sizeof(int));
	vendor[12] = '\0';
	if (std::strcmp(vendor, "GenuineIntel") == 0) info.vendor = MayaCPUInfo::Intel;
	else if (std::strcmp(vendor, "AuthenticAMD") == 0) info.vendor = MayaCPUInfo::AMD;
	else {
		info.vendor = MayaCPUInfo::Unknown;
		return; // refuses to get further information
	}

	char cpuBrandName[0x41];
	cpuBrandName[0x40] = '\0';
	// Get CPU brand name using CPUID instruction
	__cpuid(cpuInfo, 0x80000000);
	unsigned int extInfoType = cpuInfo[0];
	if (extInfoType >= 0x80000004) {
		for (unsigned int i = 0x80000002; i <= 0x80000004; ++i) {
			__cpuid(cpuInfo, i);
			std::memcpy(cpuBrandName + (i - 0x80000002) * 16, cpuInfo, sizeof(cpuInfo));
		}
	}

	info.brand = cpuBrandName;
	__cpuid(cpuInfo, 1);
	info.number_of_cores = (cpuInfo[1] >> 16) & 0xFF;
}

void MayaGetDeviceInfo(MayaRAMInfo& info)
{
#if MAYA_PLATFORM_WINDOWS
	ULONGLONG physical;
	GetPhysicallyInstalledSystemMemory(&physical);
	info.size = static_cast<unsigned>(physical);
#elif MAYA_PLATFORM_MACOS
	int mib[2];
	uint64_t ramSize;
	size_t len = sizeof(ramSize);

	// Set the sysctl MIB (Management Information Base) to query
	mib[0] = CTL_HW;       // Hardware parameters
	mib[1] = HW_MEMSIZE;  // Physical memory size

	// Query the physical memory size
	sysctl(mib, 2, &ramSize, &len, nullptr, 0);
	info.size = static_cast<unsigned>(ramSize / 1024);
#elif MAYA_PLATFORM_LINUX
	struct sysinfo sysi;
	sysinfo(&sysi);
	info.size = static_cast<unsigned>(sysi.totalram * sysi.mem_unit / 1024);
#endif
}