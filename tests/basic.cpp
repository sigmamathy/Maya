#include <Maya/core.hpp>
#include <Maya/math.hpp>
#include <Maya/deviceinfo.hpp>
#include <Maya/window.hpp>
#include <iostream>

int main()
{
	MayaInitLibrary();

	MayaWindowParameters p1;
	p1.title = "p1";
	MayaWindow w1(p1);

	MayaWindowParameters p2;
	p2.title = "p2";
	p2.detached = true;
	p2.exit_on_close = false;
	MayaWindow w2(p2);
	
	MayaWindow::RunEverything();
	

	MayaTerminateLibrary();
}