cmake_minimum_required(VERSION 3.10)

project("Maya" VERSION "1.048596")

# Assume SYSTEM=WINDOWS and vcpkg is located in C:/
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "vcpkg toolchain file")

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_library(${PROJECT_NAME}
    "src/core.cpp"
    "src/window.cpp"
    "src/deviceinfo.cpp"
    "src/shader.cpp"
    "src/vertexarray.cpp"
    "src/renderer.cpp"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
add_compile_definitions(MAYA_PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(${PROJECT_NAME} glfw glad::glad)

option(MAYA_BUILD_TESTS "Build tests" ON)
if (MAYA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()