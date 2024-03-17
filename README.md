# Maya - Library for C++

<p align="center">
  <img src="https://static.wikia.nocookie.net/aceattorney/images/4/4f/Maya_Confident_1.gif/revision/latest?cb=20120728012634" width=400>
</p>

Maya is a simple wrapper library of OpenGL and PortAudio,
provides functionality for rendering and streaming audio.

## Prerequisites

Operating system: Windows, MacOS or Linux.
Compiler: MSVC, GCC or CLANG.
Minimum version: ISO C++20
Hardware: OpenGL 3.3 supported
Git and CMake installed.

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

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
add_subdirectory("external/Maya")
target_include_directories(${PROJECT_NAME} PUBLIC "external/Maya/include")
target_link_libraries(${PROJECT_NAME} Maya)
```

Run in terminal:

```
cmake . -B out
```

## Example

The following example creates a window.

main.cpp:

```cpp
#include <maya/window.hpp>

int main()
{
	maya::LibraryManager lib;
	lib.LoadDependency(maya::GraphicsDep);
	maya::Window::uptr window = maya::Window::MakeUnique();

	while (!window->IsRequestedForClose()) {
		window->SwapBuffers();
		maya::Window::PollEvents();
	}

	return 0;
}
```