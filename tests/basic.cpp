#include <maya/gui/button.hpp>
#include <maya/gui/textfield.hpp>
#include <maya/gui/label.hpp>
#include <maya/gui/checkbox.hpp>
#include <maya/gui/panel.hpp>
#include <maya/gui/scrollbar.hpp>
#include <maya/gui/numberscroll.hpp>

int main()
{
	MayaLibraryManager manager(MAYA_LIBRARY_GLFW);
	MayaSetErrorCallback(MayaLogErrorToStdCerr);

	MayaWindowUptr window = MayaCreateWindowUptr();
	window->SetSize(1920, 1080);

	MayaGraphicsGui gui(*window);
	MayaGraphics2d g2d(*window);

	auto& pane = gui.CreateTitlePanel();
	pane.SetEnableScroll(1);
	pane.SetContentSize(MayaFvec2(800, 800));
	
	auto& r = gui.CreateNumberScroll();
	auto& g = gui.CreateNumberScroll();
	auto& b = gui.CreateNumberScroll();
	r.SetPosition(-180, 0);
	b.SetPosition(180, 0);
	r.SetRange(0, 255);
	g.SetRange(0, 255);
	b.SetRange(0, 255);
	
	pane.Add(r, g, b);

	while (!window->IsTimeToClose())
	{
		window->ClearBuffers();
		window->PackViewport();

		g2d.UseWindowProjection();
		g2d.UseColor(MayaIvec4(r.GetValue(), g.GetValue(), b.GetValue(), 255));

		gui.Draw();

		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	return 0;
}