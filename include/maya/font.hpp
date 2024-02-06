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

MayaFontUptr MayaCreateFontUptr(MayaWindow& window, void* data, unsigned size, int pixelsize);

MayaFontSptr MayaCreateFontSptr(MayaWindow& window, MayaStringCR path, int pixelsize);

MayaFontSptr MayaCreateFontSptr(MayaWindow& window, void* data, unsigned size, int pixelsize);

class MayaFont
{
public:

	MayaFont(MayaWindow& window, MayaStringCR path, int pixelsize);

	MayaFont(MayaWindow& window, void* data, unsigned size, int pixelsize);

	MayaFont(MayaFont const&) = delete;

	MayaFont& operator=(MayaFont const&) = delete;

	MayaGlyphInfo const& operator[](char c) const;

private:

	MayaHashMap<char, MayaTextureUptr> textures;
	MayaHashMap<char, MayaGlyphInfo> glyph_infos;
};

enum MayaTextAlign
{
	MayaTextAlignLeft			= 0b1,
	MayaTextAlignCenterH		= 0b10,
	MayaTextAlignRight			= 0b11,
	MayaTextAlignBottom			= 0b100,
	MayaTextAlignCenterV		= 0b1000,
	MayaTextAlignTop			= 0b1100,

	MayaTextAlignTL = MayaTextAlignTop | MayaTextAlignLeft,
	MayaTextAlignTC = MayaTextAlignTop | MayaTextAlignCenterH,
	MayaTextAlignTR = MayaTextAlignTop | MayaTextAlignRight,
	MayaTextAlignCL = MayaTextAlignCenterV | MayaTextAlignLeft,
	MayaTextAlignCC = MayaTextAlignCenterV | MayaTextAlignCenterH,
	MayaTextAlignCR = MayaTextAlignCenterV | MayaTextAlignRight,
	MayaTextAlignBL = MayaTextAlignBottom | MayaTextAlignLeft,
	MayaTextAlignBC = MayaTextAlignBottom | MayaTextAlignCenterH,
	MayaTextAlignBR = MayaTextAlignBottom | MayaTextAlignRight,
};