#pragma once

#include "./core.hpp"
#include "./math.hpp"

// Information about user's monitors
struct MayaMonitorsInfo
{
	struct components {
		int priority;
		std::string name;
		MayaIvec2 resolution;
// ----------- internal ----------- //
		void* internal_pointer;
	};
	std::vector<components>* monitors;
	int count; // number of monitors
};

// Information about user's CPU
// only supports AMD and Intel manufacturers
struct MayaCPUInfo
{
	enum manufacturers {
		AMD, Intel, // supported
		Unknown = -1
	} vendor;
	std::string brand;
	int number_of_cores;
};

struct MayaRAMInfo
{
	unsigned size;
};

void MayaGetDeviceInfo(MayaMonitorsInfo& info);

void MayaGetDeviceInfo(MayaCPUInfo& info);

void MayaGetDeviceInfo(MayaRAMInfo& info);