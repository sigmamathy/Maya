#include <maya/core.hpp>
#include <maya/window.hpp>
#include <maya/renderer.hpp>

static float vertices[] = {
	0.5f, 0.5f,
	-0.5f, 0.5f,
	-0.5f, -0.5f,
	0.5f, -0.5f
};

static unsigned indices[] = {
	0, 1, 2,
	2, 3, 0
};

int main()
{
	MayaLibraryRAII _;
	MayaSetErrorCallback([](MayaErrorStatus& err) {
		std::cout << err.Details << '\n';
	});

	MayaWindowUptr window = MayaCreateWindowUptr();

	MayaShaderProgramParameters sp;
	MayaLoadShaderFromFile(sp, MAYA_PROJECT_SOURCE_DIR "/tests/basic");
	MayaShaderProgramUptr program = MayaCreateShaderProgramUptr(*window, sp);
	program->SetUniform("uColor", 1.0f, 1.0f, 0.0f, 1.0f);

	MayaVertexArrayUptr vao = MayaCreateVertexArrayUptr(*window);
	vao->SetVertexCount(3);
	
	MayaVertexLayout layout;
	layout(0, 2);
	layout.Data = vertices;
	layout.Size = sizeof(vertices);
	vao->LinkVertexBuffer(layout);
	vao->LinkIndexBuffer(indices, sizeof(indices));

	while (!window->IsTimeToClose())
	{
		window->ClearBuffers();

		MayaRenderer r;
		r.Input = vao.get();
		r.Program = program.get();
		r.ExecuteDraw();

		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	return 0;
}