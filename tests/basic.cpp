#include <Maya/core.hpp>
#include <Maya/math.hpp>
#include <Maya/deviceinfo.hpp>
#include <Maya/window.hpp>
#include <iostream>

static MayaWindowPtr window;

static void Update(MayaWindowPtr window, MayaUpdateEvent const& e)
{
	std::cout << 1.0f / e.elapsed << '\n';
}

static void OnKeyEvent(MayaWindowPtr window, MayaKeyEvent const& e)
{
	if (e.down && e.keycode == MayaKeySpace) {
		MayaWindowParameters param;
		MayaRecreateWindow(window, param);
	}
}

int main()
{
	MayaInitLibrary();
	MayaWindowParameters param;
	param.exit_on_close = false;
	window = MayaCreateWindow(param);
	window.AddEventListener<MayaKeyEvent>(OnKeyEvent);

	MayaWindowPtr::RunEverything();
	MayaTerminateLibrary();
}