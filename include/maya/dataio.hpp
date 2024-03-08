#pragma once

#include "./math.hpp"

namespace maya
{

// Import an image froma file.
struct ImageImporter
{
	unsigned char* Data;
	Ivec2 Size;
	int Channels;

	// Import an image from a file.
	ImageImporter(char const* path, int channels = 0);

	// Free the memory.
	~ImageImporter();

	// No copy construct.
	ImageImporter(ImageImporter const&) = delete;
	ImageImporter& operator=(ImageImporter const&) = delete;
};

}