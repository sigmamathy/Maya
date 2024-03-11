#pragma once

#include "./core.hpp"
#include "./math.hpp"
#include "./texture.hpp"

namespace maya
{

struct Glyph
{
	Texture::uptr Texture = 0;
	Ivec2 Size = {0, 0}, Bearing = {0, 0};
	unsigned Advance = 0;
};

class Font
{
public:

	// Construct an empty font.
	Font();

	// Cleanup resources.
	~Font();

	// Open a font file and prepare for loading font data.
	void OpenFileStream(char const* path, int pixelsize);

	// Open a font from memory and prepare for loading font data.
	void OpenMemoryStream(ConstBuffer<void> data, int pixelsize);

	// Close the process of loading font.
	void CloseStream();

	// Create a uninitialized character glyph if not presented.
	void EmptyChar(unsigned c);

	// Load a character glyph to memory.
	void LoadChar(unsigned c, RenderContext& rc);

	// Load ASCII characters (32 to 126).
	void LoadAsciiChars(RenderContext& rc);

	// No copy construct.
	Font(Font const&) = delete;
	Font& operator=(Font const&) = delete;

	// Return true if a character glyph is loaded.
	bool IsCharLoaded(unsigned c) const;

	// Return a character glyph.
	Glyph& operator[](unsigned c);

	// Return a character glyph.
	Glyph const& operator[](unsigned c) const;

	int GetMaxHeight() const;

private:

	void *ft, *face;
	stl::hashmap<unsigned, Glyph> glyphs;
	int max_height;
};

}