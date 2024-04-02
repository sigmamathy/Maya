#include <maya/vertexarray.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace maya
{

VertexLayout::VertexLayout()
{
	attributes.reserve(8);
}

VertexLayout::VertexLayout(int location, int count)
{
	attributes.reserve(8);
	Push(location, count);
}

VertexLayout& VertexLayout::Push(int location, int count)
{
	attributes.emplace_back(location, count, stride);
	stride += count;
	return *this;
}

VertexArray::VertexArray(RenderContext& rc)
{
	Init(rc);
}

VertexArray::~VertexArray()
{
	Free();
}

VertexArray::uptr VertexArray::MakeUnique(RenderContext& rc)
{
	return uptr(new VertexArray(rc));
}

VertexArray::sptr VertexArray::MakeShared(RenderContext& rc)
{
	return sptr(new VertexArray(rc));
}

void VertexArray::Init(RenderContext& rc)
{
	RenderResource::Init(rc);
	glGenVertexArrays(1, &nativeid);
	iboid = 0;
	vertex_count = 0;
	indices_count = 0;
	draw_range = Ivec2(-1);
}

void VertexArray::Free()
{
	if (nativeid)
	{
		RenderResource::Free();
		glDeleteVertexArrays(1, &nativeid);
		if (!vboids.empty())
			glDeleteBuffers(static_cast<GLsizei>(vboids.size()), vboids.data());
		if (iboid)
			glDeleteBuffers(1, &iboid);
		nativeid = 0;
	}
}

void VertexArray::SetDrawRange(int start, int end)
{
	draw_range = { start, end };
}

void VertexArray::ResetDrawRange()
{
	draw_range = { -1, -1 };
}

Ivec2 VertexArray::GetDrawRange() const
{
	if (draw_range.x != -1)
		return draw_range;
	return iboid ? Ivec2{ 0, indices_count / 3 } : Ivec2{ 0, vertex_count };
}

template<class Ty>
void VertexArray::PushBuffer(ConstBuffer<Ty> buffer, VertexLayout& layout, bool MaySubjectToChange)
{
	unsigned& vboid = vboids.emplace_back();

	glGenBuffers(1, &vboid);
	rc->SetInput(this);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	glBufferData(GL_ARRAY_BUFFER, buffer.Size, buffer.Data,
		MaySubjectToChange ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	GLenum datatype = 0;
	if (std::is_same_v<Ty, float>)			datatype = GL_FLOAT;
	if (std::is_same_v<Ty, unsigned>)		datatype = GL_UNSIGNED_INT;
	if (std::is_same_v<Ty, int>)			datatype = GL_INT;

	for (int i = 0; i < layout.attributes.size(); i++)
	{
		auto& x = layout.attributes[i];
		glEnableVertexAttribArray(x.Location);
		glVertexAttribPointer(x.Location, x.Count, datatype, false,
			layout.stride * sizeof(Ty), (void*)(x.Offset * sizeof(Ty)));
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	MAYA_STL size_t vc = buffer.Size / layout.stride / sizeof(Ty);

	if (!vertex_count)
	{
		vertex_count = vc;
	}
#if MAYA_DEBUG
	else if(vertex_count != vc)
	{
		auto& cm = *CoreManager::Instance();
		cm.MakeError(cm.VERTEX_BUFFER_ERROR,
			"Number of vertices is inconsistence with the existsing buffers.");
	}
#endif
}

template void VertexArray::PushBuffer(Buffer<float const>, VertexLayout&, bool);
template void VertexArray::PushBuffer(Buffer<int const>, VertexLayout&, bool);
template void VertexArray::PushBuffer(Buffer<unsigned const>, VertexLayout&, bool);

unsigned VertexArray::GetBufferCount() const
{
	return static_cast<unsigned>(vboids.size());
}

void VertexArray::LinkIndexBuffer(ConstBuffer<unsigned> buffer)
{
	if (iboid)
		glDeleteBuffers(1, &iboid);
	glGenBuffers(1, &iboid);
	rc->SetInput(this);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.Size, buffer.Data, GL_STATIC_DRAW);
	rc->SetInput(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indices_count = buffer.Size / sizeof(unsigned);
}

bool VertexArray::HasIndexBuffer() const
{
	return iboid;
}

template<class Ty>
void VertexArray::UpdateBuffer(unsigned index, ConstBuffer<Ty> buffer)
{
#if MAYA_DEBUG
	if (index >= vboids.size())
	{
		auto& cm = *CoreManager::Instance();
		cm.MakeError(cm.OUT_OF_BOUNDS_ERROR,
			"Attempting to update vertex buffer that does not exists.");
		return;
	}
#endif

	auto id = vboids[index];
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.Size, buffer.Data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}