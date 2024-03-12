cmake_minimum_required(VERSION 3.10)

project("Maya" VERSION "1.048596")

# Assume SYSTEM=WINDOWS and vcpkg is located in C:/
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "vcpkg toolchain file")

# Link vcpkg packages
find_package(glfw3 CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(SndFile REQUIRED)

# Update git submodules
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodule as needed
    option(GIT_SUBMODULE "Check submodule during build" ON)
    if (GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}.")
        endif()
    endif()
endif()

# Source codes
add_library(
    ${PROJECT_NAME}
    "external/glad/src/glad.c"
    "external/stb/src/stb_image.c"

    "src/core.cpp"
    "src/deviceinfo.cpp"
    "src/window.cpp"
    "src/render.cpp"
    "src/dataio.cpp"
    "src/shader.cpp"
    "src/vertexarray.cpp"
    "src/texture.cpp"
    "src/transformation.cpp"
    "src/font.cpp"
    "src/audio.cpp"
)
 
# Setup properties
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
add_compile_definitions(MAYA_PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

# Git submodules
add_subdirectory("external/portaudio")

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC "include"
    PUBLIC "external/glad/include"
    PUBLIC "external/stb/include"
    PUBLIC "external/portaudio/include"
)

# Linked libraries
target_link_libraries(${PROJECT_NAME} glfw Freetype::Freetype SndFile::sndfile PortAudio)

# Tests
option(MAYA_BUILD_TESTS "Build tests" ON)
if (MAYA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()