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

		MayaViewport v1(w1);
		v1.SetBounds(MayaIvec4(0, 0, 500, 500));

		MayaViewport v2(w1);
		v2.SetBounds(MayaIvec4(500, 0, 500, 500));

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