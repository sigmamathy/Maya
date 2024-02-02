#pragma once

#include "./core.hpp"
#include "./math.hpp"
#include "./texture.hpp"

struct MayaGlyphInfo
{
	MayaTexture* Texture;
	MayaIvec2 Size, Bearing;
	unsigned Advance;
};

class MayaWindow;
MAYA_TYPEDEFPTR(MayaFont);

MayaFontUptr MayaCreateFontUptr(MayaWindow& window, MayaStringCR path, int pixelsize);

MayaFontSptr MayaCreateFontSptr(MayaWindow& window, MayaStringCR path, int pixelsize);

class MayaFont
{
public:

	MayaFont(MayaWindow& window, MayaStringCR path, int pixelsize);

	MayaFont(MayaFont const&) = delete;

	MayaFont& operator=(MayaFont const&) = delete;

	MayaGlyphInfo const& operator[](char c) const;

private:

	MayaHashMap<char, MayaTextureUptr> textures;
	MayaHashMap<char, MayaGlyphInfo> glyph_infos;
};