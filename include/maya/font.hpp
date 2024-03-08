#pragma once

#include "./core.hpp"
#include "./math.hpp"
#include "./texture.hpp"

namespace maya
{

struct Glyph
{
	Texture::uptr Texture;
	Ivec2 Size, Bearing;
	unsigned Advance;
};

class Font
{
public:

	Font(RenderContext& rc, char const* path, int pixelsize);

	// No copy construct.
	Font(Font const&) = delete;
	Font& operator=(Font const&) = delete;

	Glyph const& operator[](char c) const;

	int GetMaxHeight() const;

private:

	stl::hashmap<char, Glyph> glyphs;
	int max_height;
};

}