#include <maya/vertexarray.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace maya
{

VertexLayout::VertexLayout(int location, int count)
{
	Push(location, count);
}

VertexLayout& VertexLayout::Push(int location, int count)
{
	attributes.emplace_back(location, count, stride);
	stride += count;
	return *this;
}

VertexArray::VertexArray(RenderContext& rc)
	: RenderResource(rc), iboid(0), vertex_count(0), indices_count(0), draw_range(-1)
{
	glGenVertexArrays(1, &vaoid);
}

VertexArray::~VertexArray()
{
	CleanUp();
}

VertexArray::uptr VertexArray::MakeUnique(RenderContext& rc)
{
	return uptr(new VertexArray(rc));
}

VertexArray::sptr VertexArray::MakeShared(RenderContext& rc)
{
	return sptr(new VertexArray(rc));
}

void VertexArray::CleanUp()
{
	if (vaoid)
	{
		RenderResource::CleanUp();
		glDeleteVertexArrays(1, &vaoid);
		if (!vboids.empty())
			glDeleteBuffers(static_cast<GLsizei>(vboids.size()), vboids.data());
		if (iboid)
			glDeleteBuffers(1, &iboid);
		vaoid = 0;
	}
}

void RenderContext::SetInput(VertexArray* resource)
{
	if (crntvao == resource) return;
	crntvao = resource;
	glBindVertexArray(resource ? resource->vaoid : 0);
}

void VertexArray::SetDrawRange(int start, int end)
{
	draw_range = { start, end };
}

void VertexArray::ResetDrawRange()
{
	draw_range = { -1, -1 };
}

template<class Ty>
void VertexArray::PushBuffer(ConstBuffer<Ty> buffer, VertexLayout& layout, bool MaySubjectToChange)
{
	unsigned& vboid = vboids.emplace_back();

	glGenBuffers(1, &vboid);
	rc.SetInput(this);
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

	int vc = buffer.Size / layout.stride / sizeof(Ty);

	if (!vertex_count)
	{
		vertex_count = vc;
	}
	else MAYA_DIF(vertex_count != vc)
	{
		Error::Send(Error::Inconsistence,
			"VertexArray::PushBuffer(Buffer<Ty const>, VertexLayout&, bool): "
			"Number of vertices is inconsistence with the existsing buffers.");
	}
}

template void VertexArray::PushBuffer(Buffer<float const>, VertexLayout&, bool);
template void VertexArray::PushBuffer(Buffer<int const>, VertexLayout&, bool);
template void VertexArray::PushBuffer(Buffer<unsigned const>, VertexLayout&, bool);

void VertexArray::LinkIndexBuffer(ConstBuffer<unsigned> buffer)
{
	if (iboid)
		glDeleteBuffers(1, &iboid);
	glGenBuffers(1, &iboid);
	rc.SetInput(this);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.Size, buffer.Data, GL_STATIC_DRAW);
	rc.SetInput(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indices_count = buffer.Size / sizeof(unsigned);
}

template<class Ty>
void VertexArray::UpdateBuffer(int index, ConstBuffer<Ty> buffer)
{
	MAYA_DIF(index >= vboids.size())
	{
		Error::Send(Error::OutOfBounds,
			"VertexArray::UpdateBuffer(int, Buffer<Ty const>): Required buffer does not exists.");
		return;
	}

	auto id = vboids[index];
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.Size, buffer.Data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}