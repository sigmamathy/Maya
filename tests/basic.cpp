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

	MayaGraphicsGUI gui(*window);

	auto& x = gui.CreateButton();
	x.SetPositionRelativeTo(MayaGraphicsGUI::RelativeToWindowTopLeft);
	x.SetPosition(MayaFvec2(250, -150));

	float start = MayaGetCurrentTimeSinceInit();

	while (!window->IsTimeToClose())
	{
		//if (MayaGetCurrentTimeSinceInit() - start < 1 / 120.0f) continue;
		start = MayaGetCurrentTimeSinceInit();

		window->ClearBuffers();
		window->PackViewport();

		gui.Draw();

		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	return 0;
}