#include <Maya/core.hpp>
#include <Maya/math.hpp>
#include <Maya/deviceinfo.hpp>
#include <Maya/window.hpp>
#include <iostream>

int main()
{
	MayaInitLibrary();

	try {
		MayaWindowParameters p1;
		p1.title = "w1";
		MayaWindow w1;
		w1.Init(p1);

		MayaFloatingViewport v1(w1);
		v1.SetDockDirection(v1.DockBottom);
		v1.SetDockThickness(200);

		MayaFloatingViewport v2(&v1);
		v2.SetBounds(MayaIvec4(50, 50, 100, 100));

		w1.AddEventListener<MayaKeyEvent>([&](MayaWindow window, MayaKeyEvent const& e) {
			if (e.down && e.keycode == MayaKeyLeft)
				v1.SetDockThickness(v1.GetSize().y + 20);
		});

		w1.AddEventListener<MayaUpdateEvent>([&](MayaWindow window, MayaUpdateEvent const& e) {
			v1.ClearColor(MayaFvec4(1.0f, 0.0f, 0.0f, 1.0f));
			v2.ClearColor(MayaFvec4(1.0f, 1.0f, 0.0f, 1.0f));
		});

		MayaWindow::RunEverything();
	}
	catch (MayaErrorInfo e) {
		std::cout << e.details << '\n';
	}
	

	MayaTerminateLibrary();
}