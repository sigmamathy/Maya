#pragma once

#include "./core.hpp"
#include "./math.hpp"

class MayaWindow;
MAYA_TYPEDEFPTR(MayaVertexArray);

class MayaVertexLayout
{
public:
	struct Attribute {
		int Location;
		int Count;
		int Offset;
	};

	void const* Data = 0;			// REQUIRED
	unsigned Size = 0;				// REQUIRED
	bool MaySubjectToChange = 0;	// REQUIRED

	MayaVertexLayout() = default;
	MayaVertexLayout(int location, int count);
	MayaVertexLayout& operator()(int location, int count);

private:
	MayaArrayList<Attribute> attributes;
	int stride = 0;
	friend class MayaVertexArray;
};

MayaVertexArrayUptr MayaCreateVertexArrayUptr(MayaWindow& window);

MayaVertexArraySptr MayaCreateVertexArraySptr(MayaWindow& window);

class MayaVertexArray 
{
public:

	MayaVertexArray(unsigned vao, MayaWindow* window);

	~MayaVertexArray();

	MayaVertexArray(MayaVertexArray const&) = delete;

	MayaVertexArray& operator=(MayaVertexArray const&) = delete;

	void SetDrawRange(int start, int end);

	void ResetDrawRange();

	template<class Ty = float>
	void LinkVertexBuffer(MayaVertexLayout& layout);

	void LinkIndexBuffer(unsigned const* data, unsigned size);

public:

	unsigned vaoid;
	MayaWindow* window;
	MayaArrayList<unsigned> vboids;
	unsigned iboid;

	int vertex_count, indices_count;
	MayaIvec2 draw_range;

	friend class MayaRenderer;
};