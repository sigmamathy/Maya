#pragma once

#include "./core.hpp"
#include "./math.hpp"

namespace maya
{

// Information about user's monitors
struct MonitorsInfo
{
	struct MonitorData {
		int Priority;
		stl::string Name;
		Ivec2 Resolution;
		// ----------- internal ----------- //
		void* Resptr;
	};

	MonitorData* Monitors;
	int Count; // number of monitors

	// Retrieve monitors information
	static MonitorsInfo Retrieve();
};

char const* GetClipBoardString();

}