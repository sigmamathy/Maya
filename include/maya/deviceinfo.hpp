#pragma once

#include "./core.hpp"
#include "./math.hpp"

// Information about user's monitors
struct MayaMonitorsInfo
{
	struct MonitorData {
		int Priority;
		std::string Name;
		MayaIvec2 Resolution;
		// ----------- internal ----------- //
		void* Resptr;
	};
	MonitorData* Monitors;
	int Count; // number of monitors
};

void MayaGetDeviceInfo(MayaMonitorsInfo* info);

MayaString MayaGetClipBoardString();