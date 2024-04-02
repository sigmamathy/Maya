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

	// Uninitialized.
	Texture(void) = default;

	// Constructor.
	Texture(RenderContext& rc);

	// Cleanup resources.
	~Texture();

	// Create and return a uptr.
	static uptr MakeUnique(RenderContext& rc);

	// Create and return a sptr.
	static sptr MakeShared(RenderContext& rc);

	// Initialize texture.
	virtual void Init(RenderContext& rc) override;

	// Free texture.
	virtual void Free() override;

	// Create an image content of size for the texture.
	// data can be nullptr if an empty texture is desired.
	void CreateContent(void const* data, Ivec2 size, int channels);

	void SetRepeat();

	void SetClampToEdge();

	void SetFilterLinear();

	// Get the size of the texture.
	inline Ivec2 GetSize() const { return size; }

	// Get the number of channels of the texture.
	inline int GetChannels() const { return channels; }

private:

	Ivec2 size;
	int channels;

};

}