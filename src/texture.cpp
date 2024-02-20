#include <maya/texture.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

static constexpr int s_TextureFormat(int channels)
{
	switch (channels)
	{
		case 1: return GL_RED;
		case 2: return GL_RG;
		case 3: return GL_RGB;
		case 4: return GL_RGBA;
		default: return -1;
	}
}

static unsigned s_CreateTexture(void const* data, MayaIvec2 size, int channels)
{
	unsigned textureid;
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, s_TextureFormat(channels), GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
	return textureid;
}

static unsigned s_CreateTextureFromImageFile(MayaStringCR path, int channels, MayaIvec2& size)
{
	stbi_set_flip_vertically_on_load(true);
	int ch;
	stbi_uc* data = stbi_load(path.c_str(), &size.x, &size.y, &ch, channels);
	if (!data)
		return 0;
	channels = channels == 0 || ch < channels ? ch : channels;
	unsigned id = s_CreateTexture(data, size, channels);
	stbi_image_free(data);
	return id;
}

MayaTexture* s_CreateTexturePtr(MayaWindow& window, MayaTextureParameters& param)
{
	window.UseGraphicsContext();

	if (param.Source == MayaTextureParameters::FromMemory)
	{
		unsigned id = s_CreateTexture(param.RawData.Data, param.RawData.Size, param.RawData.Channels);
		return new MayaTexture(id, &window, param.RawData.Size);
	}
	else
	{
		MayaIvec2 size;
		unsigned id = s_CreateTextureFromImageFile(param.FileLoad.Path, param.FileLoad.Channels, size);
		return id ? new MayaTexture(id, &window, size) : nullptr;
	}
}

MayaTextureUptr MayaCreateTextureUptr(MayaWindow& window, MayaTextureParameters& param)
{
	auto ptr = s_CreateTexturePtr(window, param);
	if (!ptr) [[unlikely]] {
		MayaSendError({ MAYA_IMAGE_LOAD_ERROR,
					"MayaCreateTextureUptr(MayaWindow&, MayaTextureParameters&): "
					"The required file path does not exists or unsupported format is detected." });
		return nullptr;
	}
	return MayaTextureUptr(ptr);
}

MayaTextureSptr MayaCreateTextureSptr(MayaWindow& window, MayaTextureParameters& param)
{
	auto ptr = s_CreateTexturePtr(window, param);
	if (!ptr) [[unlikely]] {
		MayaSendError({ MAYA_IMAGE_LOAD_ERROR,
					"MayaCreateTextureSptr(MayaWindow&, MayaTextureParameters&): "
					"The required file path does not exists or unsupported format is detected." });
		return nullptr;
	}
	return MayaTextureSptr(ptr);
}

MayaTexture::MayaTexture(unsigned texture, MayaWindow* window, MayaIvec2 size)
	: textureid(texture), window(window), size(size)
{
}

MayaTexture::~MayaTexture()
{
	if (!MayaWindow::Exists(window))
		return;
	window->UseGraphicsContext();
	glDeleteTextures(1, &textureid);
}

MayaIvec2 MayaTexture::GetSize() const
{
	return size;
}