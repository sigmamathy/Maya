#include <maya/texture.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

namespace maya
{

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

Texture::Texture(RenderContext& rc)
	: RenderResource(rc)
{
	glGenTextures(1, &textureid);
}

Texture::~Texture()
{
	CleanUp();
}

Texture::uptr Texture::MakeUnique(RenderContext& rc)
{
	return uptr(new Texture(rc));
}

Texture::sptr Texture::MakeShared(RenderContext& rc)
{
	return sptr(new Texture(rc));
}

void Texture::CleanUp()
{
	if (textureid)
	{
		RenderResource::CleanUp();
		glDeleteTextures(1, &textureid);
		textureid = 0;
	}
}

void RenderContext::SetTexture(Texture* tex, int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex ? tex->textureid : 0);
}

void Texture::CreateContent(void const* data, Ivec2 size, int channels)
{
	rc.SetTexture(this, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		size.x, size.y, 0, s_TextureFormat(channels), GL_UNSIGNED_BYTE, data);
	rc.SetTexture(0, 0);
}

static unsigned s_CreateTexture(void const* data, Ivec2 size, int channels, bool repeat)
{
	unsigned textureid;
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, s_TextureFormat(channels), GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
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