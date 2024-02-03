#include <maya/core.hpp>
#include <maya/window.hpp>
#include <maya/renderer.hpp>
#include <maya/transformation.hpp>
#include <maya/2d/graphics.hpp>
#include <maya/2d/renderer.hpp>

int main()
{
	MayaLibrarySingleton _;
	MayaSetErrorCallback([](MayaErrorStatus& err) {
		std::cout << err.Details << '\n';
	});

	MayaWindowUptr window = MayaCreateWindowUptr();
	window->SetResizeAspectRatioLock(16, 9);
	MayaGraphics2D& g2d = window->GetGraphics2D();

	MayaTextureUptr texture = MayaCreateTextureUptrFromImageFile(*window, MAYA_PROJECT_SOURCE_DIR "/tests/image0.png");

	while (!window->IsTimeToClose())
	{
		window->ClearBuffers();
		window->PackViewport();

		g2d.UseWindowProjection();
		g2d.UseColor(0xFF0000);
		g2d.UseTexture(texture.get());
		g2d.DrawRect(MayaIvec2(-100, 0), MayaIvec2(300));

		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	return 0;
}