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

	auto& pane = gui.CreateTitlePanel();
	//pane.SetEnabled(false);

	//auto& button = gui.CreateButton();
	//button.SetPosition(100, 250);
	//pane.Add(button);
	
	//auto& label = gui.CreateLabel();

	auto& textfield = gui.CreateTextField();
	textfield.SetSize(500, 60);
	pane.Add(textfield);

	auto& x = gui.CreateNumberScroll();
	x.SetPosition(0, 100);
	pane.Add(x);

	while (!window->IsTimeToClose())
	{
		window->ClearBuffers();
		window->PackViewport();

		gui.Draw();

		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	return 0;
}