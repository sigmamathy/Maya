#include <maya/texture.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

namespace maya
{

static constexpr GLenum s_TextureFormat(int channels)
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

Texture::Texture(RenderContext& rc)
{
	Init(rc);
}

Texture::~Texture()
{
	Free();
}

Texture::uptr Texture::MakeUnique(RenderContext& rc)
{
	return uptr(new Texture(rc));
}

Texture::sptr Texture::MakeShared(RenderContext& rc)
{
	return sptr(new Texture(rc));
}

void Texture::Init(RenderContext& rc)
{
	RenderResource::Init(rc);
	glGenTextures(1, &nativeid);
	size = { 0, 0 };
	channels = 0;
}

void Texture::Free()
{
	if (nativeid) {
		RenderResource::Free();
		glDeleteTextures(1, &nativeid);
		nativeid = 0;
	}
}

void Texture::CreateContent(void const* data, Ivec2 size, int channels)
{
	rc->SetTexture(this, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
		size.x, size.y, 0, s_TextureFormat(channels), GL_UNSIGNED_BYTE, data);
	rc->SetTexture(0, 0);
}

void Texture::SetRepeat()
{
	glBindTexture(GL_TEXTURE_2D, nativeid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::SetClampToEdge()
{
	glBindTexture(GL_TEXTURE_2D, nativeid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::SetFilterLinear()
{
	glBindTexture(GL_TEXTURE_2D, nativeid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

}