#pragma once

#include "./core.hpp"
#include "./math.hpp"

class MayaWindow;
MAYA_TYPEDEFPTR(MayaVertexArray);

template<class Ty>
struct MayaBuffer
{
	Ty const* Data = 0;
	unsigned Size = 0;
};

class MayaVertexLayout
{
public:
	struct Attribute {
		int Location;
		int Count;
		int Offset;
	};

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

	template<class Ty>
	void LinkVertexBuffer(MayaBuffer<Ty> buffer, MayaVertexLayout& layout, bool MaySubjectToChange = false);

	void LinkIndexBuffer(MayaBuffer<unsigned> buffer);

	template<class Ty>
	void UpdateVertexBuffer(int index, MayaBuffer<Ty> buffer);

public:

	unsigned vaoid;
	MayaWindow* window;
	MayaArrayList<unsigned> vboids;
	unsigned iboid;

	int vertex_count, indices_count;
	MayaIvec2 draw_range;

	friend class MayaRenderer;
};