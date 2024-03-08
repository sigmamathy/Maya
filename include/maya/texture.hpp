#pragma once

#include "./render.hpp"

namespace maya
{

class Texture : public RenderResource
{
public:

	using uptr = stl::uptr<Texture>;
	using sptr = stl::sptr<Texture>;

	Texture(RenderContext& rc);

	~Texture();

	static uptr MakeUnique(RenderContext& rc);

	static sptr MakeShared(RenderContext& rc);

	void CleanUp() override;

	void CreateContent(void const* data, Ivec2 size, int channels);

	void SetRepeat();

	void SetFilterLinear();

	Ivec2 GetSize() const;

	int GetChannels() const;

private:

	unsigned textureid;
	Ivec2 size;
	int channels;
	friend class RenderContext;

};

}