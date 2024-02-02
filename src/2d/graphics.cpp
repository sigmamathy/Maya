#include "./internal.hpp"

MayaHashMap<MayaWindow*, Maya_s_Graphics2D_Resources> Maya_s_g2d_resources;

MayaGraphics2D::SingletonPerWindow::SingletonPerWindow(MayaWindow& window)
	: Window(&window)
{
	MAYA_DIF(Maya_s_g2d_resources.count(&window)) {
		MAYA_SERR(MAYA_SINGLETON_ERROR,
			"MayaGraphics2D::SingletonPerWindow::SingletonPerWindow(MayaWindow&): "
			"Multiple instances of MayaGraphics2D::SingletonPerWindow found for a single window context.");
		return;
	}

	auto& x = Maya_s_g2d_resources[&window];
	MayaShaderProgramParameters param;
	MayaLoadShaderFromFile(param, MAYA_PROJECT_SOURCE_DIR "/src/2d/0");
	x.Program = MayaCreateShaderProgramUptr(window, param);

	static float square_vertices[] = {
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, 0.5f,
		0.5f, -0.5f
	};

	x.SquareVAO = MayaCreateVertexArrayUptr(window);
	x.SquareVAO->SetVertexCount(6);
	MayaVertexLayout layout (0, 2);
	layout.Data = square_vertices;
	layout.Size = sizeof(square_vertices);
	x.SquareVAO->LinkVertexBuffer(layout);
}

MayaGraphics2D::SingletonPerWindow::~SingletonPerWindow()
{
	Maya_s_g2d_resources.erase(Window);
}