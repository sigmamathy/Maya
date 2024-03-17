#pragma once

#include "./math.hpp"

namespace maya
{

// Stores image data.
struct ImageData
{
	// Uncompressed byte image data.
	stl::list<unsigned char> Data;

	// Image dimension.
	Ivec2 Size;

	// Number of channels.
	int Channels;

	// Import image data from file.
	void ImportFile(char const* path, int channels = 0);
};

// Stores font data.
struct FontData
{
	// Character glyph data.
	struct Glyph
	{
		stl::uptr<class Texture> Texture;
		Ivec2 Size, Bearing;
		unsigned Advance;
	};

	// List of glyphs.
	stl::hashmap<unsigned, Glyph> Data;

	// Import font data from file.
	void ImportFile(char const* path, int pixelsize, class RenderContext& rc);

	// Import font data from memory.
	void ImportMemory(ConstBuffer<void> data, int pixelsize, class RenderContext& rc);
};

// Stores audio data.
struct AudioData
{
	// Sample data
	stl::list<float> Samples;

	// Sample Rate in Hz
	unsigned SampleRate;

	// Number of channels
	unsigned Channels;

	// Read audio source from a audio file.
	// Support WAV and MP3 loading.
	void ImportFile(char const* path);
};

}