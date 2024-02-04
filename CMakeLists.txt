cmake_minimum_required(VERSION 3.10)

project("Maya" VERSION "1.048596")

# Assume SYSTEM=WINDOWS and vcpkg is located in C:/
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "vcpkg toolchain file")

find_package(glfw3 CONFIG REQUIRED)
find_package(Freetype REQUIRED)

add_library(${PROJECT_NAME}
    "external/glad/src/glad.c"
    "external/stb/src/stb_image.c"
    "src/core.cpp"
    "src/window.cpp"
    "src/deviceinfo.cpp"
    "src/shader.cpp"
    "src/vertexarray.cpp"
    "src/renderer.cpp"
    "src/texture.cpp"
    "src/transformation.cpp"
    "src/font.cpp"
    "src/2d/graphics.cpp" "src/2d/renderer.cpp" "src/color.cpp" "src/2d/camera.cpp" "src/2d/textdisplay.cpp")
 
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
add_compile_definitions(MAYA_PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME} PUBLIC "include" "external/glad/include" "external/stb/include")
target_link_libraries(${PROJECT_NAME} glfw Freetype::Freetype)

option(MAYA_BUILD_TESTS "Build tests" ON)
if (MAYA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()