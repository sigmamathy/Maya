#include <maya/core.hpp>
#include <maya/window.hpp>
#include <maya/shader.hpp>

int main()
{
	MayaInitLibrary();
	MayaSetErrorCallback([](MayaErrorStatus& err) {
		std::cout << err.Details << '\n';
	});

	MayaWindowUptr window = MayaCreateWindowUptr();

	MayaShaderProgramParameters sp;
	MayaLoadShaderFromFile(sp, "D:/Files/dev/vsproject/Maya/tests/basic");
	MayaShaderProgramUptr program = MayaCreateShaderProgramUptr(*window, sp);

	while (!window->IsTimeToClose())
	{
		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	MayaTerminateLibrary();
	return 0;
}