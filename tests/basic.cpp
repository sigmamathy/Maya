#include <maya/core.hpp>
#include <maya/window.hpp>
#include <maya/renderer.hpp>
#include <maya/transformation.hpp>

static float vertices[] = {
	0.5f, 0.5f, 0.0f,			1, 1,
	-0.5f, 0.5f, 0.0f,			0, 1,
	-0.5f, -0.5f, 0.0f,			0, 0,
	0.5f, -0.5f, 0.0f,			1, 0
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
	window->SetResizeAspectRatioLock(16, 9);

	MayaShaderProgramParameters sp;
	MayaLoadShaderFromFile(sp, MAYA_PROJECT_SOURCE_DIR "/tests/basic");
	MayaShaderProgramUptr program = MayaCreateShaderProgramUptr(*window, sp);
	program->SetUniform<int>("uTextureSlot0", 0);
	program->SetUniformMatrix("uModel", MayaFmat4(1));
	program->SetUniformMatrix("uView", MayaLookAt(MayaFvec3(0, 0, -1), MayaFvec3(1, 0, 1)));
	program->SetUniformMatrix("uProjection", MayaPerspectiveProjection(1.5707963268f, 16.0f / 9, 0.1f, 100.0f));

	MayaVertexArrayUptr vao = MayaCreateVertexArrayUptr(*window);
	vao->SetVertexCount(4);
	
	MayaVertexLayout layout;
	layout(0, 3)(1, 2);
	layout.Data = vertices;
	layout.Size = sizeof(vertices);
	vao->LinkVertexBuffer(layout);
	vao->LinkIndexBuffer(indices, sizeof(indices));

	MayaTextureUptr texture = MayaCreateTextureUptrFromImageFile(*window, MAYA_PROJECT_SOURCE_DIR "/tests/image0.png");

	while (!window->IsTimeToClose())
	{
		window->ClearBuffers();
		window->PackViewport();

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