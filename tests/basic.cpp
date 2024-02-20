#include <maya/gui/button.hpp>
#include <maya/gui/textfield.hpp>
#include <maya/gui/label.hpp>
#include <maya/gui/checkbox.hpp>
#include <maya/gui/panel.hpp>

int main()
{
	MayaLibraryManager manager(MAYA_LIBRARY_GLFW);

	MayaSetErrorCallback([](MayaError& err) {
		std::cout << err.Details << '\n';
	});

	MayaWindowUptr window = MayaCreateWindowUptr();
	window->SetSize(1920, 1080);

	MayaGraphicsGui gui(*window);
	auto& button = gui.CreateButton();

	auto& pane = gui.CreatePanel();
	pane.Add(button);
	button.SetPosition(350, -100);
	
	//auto& label = gui.CreateLabel();
	//auto& textfield = gui.CreateTextField();
	//auto& checkbox = gui.CreateCheckbox();
	
	//label.SetPosition(0, 100);
	//button.SetPosition(-100, -100);
	//checkbox.SetPosition(100, -100);

	//textfield.SetEventCallback([&](MayaEventGui& e) -> void {
	//	if (e.Type == e.Interact)
	//		label.SetText(textfield.GetText());
	//});

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