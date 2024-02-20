#include <maya/vertexarray.hpp>
#include <maya/window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

MayaVertexLayout::MayaVertexLayout(int location, int count)
{
	this->operator()(location, count);
}

MayaVertexLayout& MayaVertexLayout::operator()(int location, int count)
{
	attributes.emplace_back(location, count, stride);
	stride += count;
	return *this;
}

MayaVertexArrayUptr MayaCreateVertexArrayUptr(MayaWindow& window)
{
	window.UseGraphicsContext();
	unsigned vaoid;
	glGenVertexArrays(1, &vaoid);
	return std::make_unique<MayaVertexArray>(vaoid, &window);
}

MayaVertexArraySptr MayaCreateVertexArraySptr(MayaWindow& window)
{
	window.UseGraphicsContext();
	unsigned vaoid;
	glGenVertexArrays(1, &vaoid);
	return std::make_shared<MayaVertexArray>(vaoid, &window);
}

MayaVertexArray::MayaVertexArray(unsigned vao, MayaWindow* window)
	: vaoid(vao), window(window), vertex_count(0), iboid(0), indices_count(0), draw_range(-1)
{
	vboids.reserve(1);
}

MayaVertexArray::~MayaVertexArray()
{
	if (!MayaWindow::Exists(window))
		return;
	window->UseGraphicsContext();
	glDeleteVertexArrays(1, &vaoid);
	if (!vboids.empty())
		glDeleteBuffers((GLsizei)vboids.size(), vboids.data());
	if (iboid)
		glDeleteBuffers(1, &iboid);
}

void Maya_s_BindVertexArray(MayaWindow* window, unsigned vaoid);

void MayaVertexArray::SetDrawRange(int start, int end)
{
	draw_range = { start, end };
}

void MayaVertexArray::ResetDrawRange()
{
	draw_range = { -1, -1 };
}

template<class Ty>
void MayaVertexArray::LinkVertexBuffer(MayaBuffer<Ty> buffer, MayaVertexLayout& layout, bool MaySubjectToChange)
{
	window->UseGraphicsContext();
	unsigned& vboid = vboids.emplace_back();

	glGenBuffers(1, &vboid);
	glBindVertexArray(vaoid);
	Maya_s_BindVertexArray(window, vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	glBufferData(GL_ARRAY_BUFFER, buffer.Size, buffer.Data,
		MaySubjectToChange ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	
	GLenum datatype;
	if (std::is_same_v<Ty, float>)			datatype = GL_FLOAT;
	else if (std::is_same_v<Ty, unsigned>)	datatype = GL_UNSIGNED_INT;
	else if (std::is_same_v<Ty, int>)		datatype = GL_INT;
	
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
		MayaSendError({ MAYA_INCONSISTENTENCY_ERROR,
			"MayaVertexArray::LinkVertexBuffer(MayaVertexLayout&): "
			"Number of vertices is inconsistence with the existsing buffers." });
	}
}

template void MayaVertexArray::LinkVertexBuffer(MayaBuffer<float>, MayaVertexLayout&, bool);
template void MayaVertexArray::LinkVertexBuffer(MayaBuffer<int>, MayaVertexLayout&, bool);
template void MayaVertexArray::LinkVertexBuffer(MayaBuffer<unsigned>, MayaVertexLayout&, bool);

void MayaVertexArray::LinkIndexBuffer(MayaBuffer<unsigned> buffer)
{
	window->UseGraphicsContext();
	if (iboid)
		glDeleteBuffers(1, &iboid);
	glGenBuffers(1, &iboid);
	Maya_s_BindVertexArray(window, vaoid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.Size, buffer.Data, GL_STATIC_DRAW);
	Maya_s_BindVertexArray(window, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indices_count = buffer.Size / sizeof(unsigned);
}

template<class Ty>
void MayaVertexArray::UpdateVertexBuffer(int index, MayaBuffer<Ty> buffer)
{
	MAYA_DIF(index >= vboids.size())
	{
		MayaSendError({ MAYA_BOUNDARY_ERROR,
			"MayaVertexArray::UpdateVertexBuffer(int, MayaBuffer<Ty>): Required buffer does not exists." });
		return;
	}

	window->UseGraphicsContext();
	auto id = vboids[index];
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.Size, buffer.Data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}