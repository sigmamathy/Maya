#include <maya/core.hpp>
#include <maya/window.hpp>
#include <maya/renderer.hpp>

static float vertices[] = {
	0.5f, 0.5f,			1, 1,
	-0.5f, 0.5f,		0, 1,
	-0.5f, -0.5f,		0, 0,
	0.5f, -0.5f,		1, 0
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
	program->SetUniform<int>("uTextureSlot0", 0);

	MayaVertexArrayUptr vao = MayaCreateVertexArrayUptr(*window);
	vao->SetVertexCount(4);
	
	MayaVertexLayout layout;
	layout(0, 2)(1, 2);
	layout.Data = vertices;
	layout.Size = sizeof(vertices);
	vao->LinkVertexBuffer(layout);
	vao->LinkIndexBuffer(indices, sizeof(indices));

	MayaTextureUptr texture = MayaCreateTextureUptrFromImageFile(*window, MAYA_PROJECT_SOURCE_DIR "/tests/image0.png");

	while (!window->IsTimeToClose())
	{
		window->ClearBuffers();

		MayaRenderer r;
		r.Input			= vao.get();
		r.Program		= program.get();
		r.Textures[0]	= texture.get();
		r.ExecuteDraw();

		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	return 0;
}