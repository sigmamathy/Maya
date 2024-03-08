#pragma once

#include "./render.hpp"

namespace maya
{

template<class Ty>
struct Buffer
{
	Ty* Data = 0;
	unsigned Size = 0;
};

template<class Ty>
using ConstBuffer = Buffer<Ty const>;

class VertexLayout
{
public:
	struct Attribute {
		int Location;
		int Count;
		int Offset;
	};

	VertexLayout() = default;
	VertexLayout(int location, int count);
	VertexLayout& Push(int location, int count);

private:
	stl::list<Attribute> attributes;
	int stride = 0;
	friend class VertexArray;
};

class VertexArray : public RenderResource
{
public:

	using uptr = stl::uptr<VertexArray>;
	using sptr = stl::sptr<VertexArray>;

	VertexArray(RenderContext& rc);

	~VertexArray();

	// No copy construct.
	VertexArray(VertexArray const&) = delete;
	VertexArray& operator=(VertexArray const&) = delete;

	static uptr MakeUnique(RenderContext& rc);

	static sptr MakeShared(RenderContext& rc);

	void CleanUp() override;

	template<class Ty>
	void PushBuffer(ConstBuffer<Ty> buffer, VertexLayout& layout, bool MaySubjectToChange = false);

	void LinkIndexBuffer(ConstBuffer<unsigned> buffer);

	void SetDrawRange(int start, int end);

	void ResetDrawRange();

	template<class Ty>
	void UpdateBuffer(int index, ConstBuffer<Ty> buffer);

public:

	unsigned vaoid;
	stl::list<unsigned> vboids;
	unsigned iboid;

	int vertex_count, indices_count;
	Ivec2 draw_range;
	friend class RenderContext;
};

}