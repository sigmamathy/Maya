#pragma once

#include "./core.hpp"
#include "./math.hpp"

class MayaWindow;
MAYA_TYPEDEFPTR(MayaTexture);

MayaTextureUptr MayaCreateTextureUptr(MayaWindow& window, void const* data, MayaIvec2 size, int channels);

MayaTextureUptr MayaCreateTextureUptrFromImageFile(MayaWindow& window, MayaStringCR path, int channels = 0);

MayaTextureSptr MayaCreateTextureSptr(MayaWindow& window, void const* data, MayaIvec2 size, int channels);

MayaTextureSptr MayaCreateTextureSptrFromImageFile(MayaWindow& window, MayaStringCR path, int channels = 0);

class MayaTexture
{
public:

	MayaTexture(unsigned texture, MayaWindow* window, MayaIvec2 size);

	~MayaTexture();

	MayaTexture(MayaTexture const&) = delete;

	MayaTexture& operator=(MayaTexture const&) = delete;

	MayaIvec2 GetSize() const;

private:

	unsigned textureid;
	MayaWindow* window;
	MayaIvec2 size;
	friend class MayaRenderer;

};