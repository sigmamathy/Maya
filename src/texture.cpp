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
	: RenderResource(rc), size(0), channels(0)
{
	rc.Begin();
	glGenTextures(1, &textureid);
}

Texture::~Texture()
{
	Destroy();
}

Texture::uptr Texture::MakeUnique(RenderContext& rc)
{
	return uptr(new Texture(rc));
}

Texture::sptr Texture::MakeShared(RenderContext& rc)
{
	return sptr(new Texture(rc));
}

void Texture::Destroy()
{
	if (textureid)
	{
		RenderResource::Destroy();
		glDeleteTextures(1, &textureid);
		textureid = 0;
	}
}

void RenderContext::SetTexture(Texture* tex, int slot)
{
	if (textures[slot] == tex) return;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex ? tex->GetNativeId() : 0);
	textures[slot] = tex;
}

void Texture::CreateContent(void const* data, Ivec2 size, int channels)
{
	rc.SetTexture(this, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
		size.x, size.y, 0, s_TextureFormat(channels), GL_UNSIGNED_BYTE, data);
	rc.SetTexture(0, 0);
}

unsigned Texture::GetNativeId() const
{
	return textureid;
}

void Texture::SetRepeat()
{
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::SetClampToEdge()
{
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::SetFilterLinear()
{
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Ivec2 Texture::GetSize() const
{
	return size;
}

int Texture::GetChannels() const
{
	return channels;
}

}