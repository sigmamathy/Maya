#pragma once

#include "./core.hpp"

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

	void* Data;
	unsigned Size;

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

	MayaVertexArray(int vao, MayaWindow* window);

	~MayaVertexArray();

	void SetVertexCount(int count);

	template<class Ty = float>
	void LinkVertexBuffer(MayaVertexLayout& layout);

	void LinkIndexBuffer(unsigned* data, unsigned size);

public:

	unsigned vaoid;
	MayaWindow* window;
	MayaArrayList<unsigned> vboids;
	int vertex_count;
	unsigned iboid;
	int indices_draw_count;

	friend class MayaRenderer;
};