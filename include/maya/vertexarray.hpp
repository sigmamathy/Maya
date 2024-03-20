#pragma once

#include "./render.hpp"

namespace maya
{

class VertexLayout
{
public:
	struct Attribute {
		int Location;
		int Count;
		int Offset;
	};

	VertexLayout();
	VertexLayout(int location, int count);
	VertexLayout& Push(int location, int count);

private:
	stl::list<Attribute> attributes;
	int stride = 0;
	friend class VertexArray;
};

// Collection of vertex buffers and index buffer.
class VertexArray : public RenderResource
{
public:

	using uptr = stl::uptr<VertexArray>;
	using sptr = stl::sptr<VertexArray>;

	// Constructor.
	VertexArray(RenderContext& rc);

	// Cleanup resources.
	~VertexArray();

	// No copy construct.
	VertexArray(VertexArray const&) = delete;
	VertexArray& operator=(VertexArray const&) = delete;

	// Create and return a uptr.
	static uptr MakeUnique(RenderContext& rc);

	// Create and return a sptr.
	static sptr MakeShared(RenderContext& rc);

	// Return the vertex array id.
	unsigned GetNativeId() const override;

	// Add a vertex buffer to the array.
	template<class Ty>
	void PushBuffer(ConstBuffer<Ty> buffer, VertexLayout& layout, bool MaySubjectToChange = false);

	// Return the number of vertex buffers.
	unsigned GetBufferCount() const;

	// Link an index buffer to the array.
	void LinkIndexBuffer(ConstBuffer<unsigned> buffer);

	// Return true if a index buffer is linked.
	bool HasIndexBuffer() const;

	// Indicates which range of vertices should be involved in rendering.
	void SetDrawRange(int start, int end);

	// Reset the draw range to default.
	void ResetDrawRange();

	// Get the draw range.
	Ivec2 GetDrawRange() const;

	// Update a specific vertex buffer.
	template<class Ty>
	void UpdateBuffer(unsigned index, ConstBuffer<Ty> buffer);

protected:

	void Destroy() override;

private:

	unsigned vaoid;
	stl::list<unsigned> vboids;
	unsigned iboid;

	int vertex_count, indices_count;
	Ivec2 draw_range;
};

}