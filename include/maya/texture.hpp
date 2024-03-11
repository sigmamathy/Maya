#pragma once

#include "./render.hpp"

namespace maya
{

// 2D Texture for rendering.
class Texture : public RenderResource
{
public:

	using uptr = stl::uptr<Texture>;
	using sptr = stl::sptr<Texture>;

	// Constructor.
	Texture(RenderContext& rc);

	// Cleanup resources.
	~Texture();

	// Create and return a uptr.
	static uptr MakeUnique(RenderContext& rc);

	// Create and return a sptr.
	static sptr MakeShared(RenderContext& rc);

	// Create an image content of size for the texture.
	// data can be nullptr if an empty texture is desired.
	void CreateContent(void const* data, Ivec2 size, int channels);

	// Return the texture id.
	unsigned GetNativeId() const override;

	void SetRepeat();

	void SetClampToEdge();

	void SetFilterLinear();

	// Get the size of the texture.
	Ivec2 GetSize() const;

	// Get the number of channels of the texture.
	int GetChannels() const;

protected:

	void Destroy() override;

private:

	unsigned textureid;
	Ivec2 size;
	int channels;

};

}