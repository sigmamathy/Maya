#pragma once

#include "./core.hpp"
#include "./math.hpp"

class MayaWindow;
MAYA_TYPEDEFPTR(MayaTexture);

struct MayaTextureParameters
{
	union {
		struct {
			void const* Data;
			MayaIvec2 Size;
			int Channels;
		} RawData;
		struct {
			char const* Path;
			int Channels;
		} FileLoad;
	};
	enum DataSource {
		FromMemory, FromFile
	} Source;

	bool Repeat = true;
};

MayaTextureUptr MayaCreateTextureUptr(MayaWindow& window, MayaTextureParameters& param);

MayaTextureSptr MayaCreateTextureSptr(MayaWindow& window, MayaTextureParameters& param);

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