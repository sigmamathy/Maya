# Maya - A comprehensive library for rendering and more

Maya is a library written in C++ focuses on graphics rendering and audio streaming,
based on verbose C API such as OpenGL and PortAudio.
It is designed to be easy to use, high extensionability, maximize efficency
while offers full controll over resource management.

## Prerequisites

Operating system needs to be either Windows, MacOS or Linux.
Supported compilers are MSVC, GCC or CLANG.

Also make sure the following is properly setup:

1. CMake installed in the system

2. Vcpkg and install the following packages for dependencies:

	- glfw3 (x64)
	- Freetype (x64)
	- PortAudio (x64)

## Installation - CMake

Uses git submodule to clone the repository. i.e.

```
git submodule add https://github.com/sigmamathy/Maya.git external/Maya
```

In CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.10)

project(MyProject)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory("external/Maya")
target_include_directories(${PROJECT_NAME} "external/Maya/include")
target_link_libraries(${PROJECT_NAME} Maya)
```

Replace `PATH` with your vcpkg toolchain filepath and run:

```
cmake . -B out -DCMAKE_TOOLCHAIN_FILE="PATH"
```

## Example

The following example creates a window and a centered button.

main.cpp:

```cpp
#include <maya/window.hpp>
#include <maya/gui/button.hpp>

int main()
{
	MayaLibrarySingleton _;

	MayaWindowUptr window = MayaCreateWindowUptr();

	MayaGraphicsGUI gui(*window);
	gui.CreateButton();

	while (!window->IsTimeToClose())
	{
		window->ClearBuffers();
		gui.Draw();
		window->SwapBuffers();
		MayaPollWindowEvents();
	}

	return 0;
}
```