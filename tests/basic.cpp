#include <maya/core.hpp>
#include <maya/window.hpp>
#include <maya/renderer.hpp>
#include <maya/transformation.hpp>
#include <maya/2d/graphics.hpp>
#include <maya/2d/camera.hpp>
#include <maya/2d/textdisplay.hpp>
#include <maya/gui/button.hpp>

int main()
{
	MayaLibrarySingleton _;
	MayaSetErrorCallback([](MayaErrorStatus& err) {
		std::cout << err.Details << '\n';
	});

	MayaWindowUptr window = MayaCreateWindowUptr();
	window->SetResizeAspectRatioLock(16, 9);
	MayaGraphics2D& g2d = window->GetGraphics2D();
	MayaGraphicsGUI& gui = window->GetGraphicsGUI();

	gui.CreateButton();

	MayaGraphics2D::Camera cam;
	g2d.UseCamera(&cam);

	MayaTextureUptr texture = MayaCreateTextureUptrFromImageFile(*window, MAYA_PROJECT_SOURCE_DIR "/tests/image0.png");
	MayaFontUptr font = MayaCreateFontUptr(*window, MAYA_PROJECT_SOURCE_DIR "/tests/Arial.ttf", 50);

	MayaGraphics2D::TextDisplay text(*font, "Hello World");
	text.SetTextAlign(MayaTextAlignCC);

	float start = MayaGetCurrentTimeSinceInit();

	while (!window->IsTimeToClose())
	{
		//if (MayaGetCurrentTimeSinceInit() - start < 1 / 120.0f) continue;
		start = MayaGetCurrentTimeSinceInit();

		window->ClearBuffers();
		window->PackViewport();

		g2d.UseWindowProjection();
		//g2d.UseTexture(texture.get());
		//g2d.UseColor(0xFF0000);
		//g2d.DrawOval(MayaIvec2(-100, 0), MayaIvec2(100));
		//
		//g2d.UseColor(0x0000FF);
		//g2d.DrawRect(MayaIvec2(100, 0), MayaIvec2(100));

		//g2d.UseColor(0xFF00FF);
		g2d.DrawText(text);

		//g2d.DrawLine(-100, -100, 100, 100);

		gui.Draw();

		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	return 0;
}