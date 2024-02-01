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

MayaVertexArray::MayaVertexArray(int vao, MayaWindow* window)
	: vaoid(vao), window(window), vertex_count(0)
{
	vboids.reserve(1);
}

MayaVertexArray::~MayaVertexArray()
{
	window->UseGraphicsContext();
	glDeleteVertexArrays(1, &vaoid);
	if (!vboids.empty())
		glDeleteBuffers((GLsizei)vboids.size(), vboids.data());
}

unsigned Maya_s_binded_vao = 0;

static void s_BindVertexArray(unsigned vaoid)
{
	if (Maya_s_binded_vao = vaoid) return;
	glBindVertexArray(vaoid);
	Maya_s_binded_vao = vaoid;
}

void MayaVertexArray::SetVertexCount(int count)
{
	vertex_count = count;
}

template<class Ty>
void MayaVertexArray::LinkVertexBuffer(MayaVertexLayout& layout)
{
	window->UseGraphicsContext();
	unsigned vboid;
	glGenBuffers(1, &vboid);
	s_BindVertexArray(vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	glBufferData(GL_ARRAY_BUFFER, layout.Size, layout.Data, GL_STATIC_DRAW);

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

	vboids.push_back(vboid);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template void MayaVertexArray::LinkVertexBuffer<float>(MayaVertexLayout&);
template void MayaVertexArray::LinkVertexBuffer<int>(MayaVertexLayout&);
template void MayaVertexArray::LinkVertexBuffer<unsigned>(MayaVertexLayout&);