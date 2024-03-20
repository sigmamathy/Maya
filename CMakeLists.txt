cmake_minimum_required(VERSION 3.10)

project("Maya" VERSION "1.048596")

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
    "external/minimp3/src/minimp3.c"

    "src/core.cpp"
    "src/deviceinfo.cpp"
    "src/window.cpp"
    "src/render.cpp"
    "src/dataio.cpp"
    "src/shader.cpp"
    "src/vertexarray.cpp"
    "src/texture.cpp"
    "src/transformation.cpp"
    
    "src/audio.cpp"
 "src/async.cpp")
 
# Setup properties
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_DOCS OFF)
set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_BROTLI ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)

# Git submodules
add_subdirectory("external/glfw")
add_subdirectory("external/freetype")
add_subdirectory("external/portaudio")
add_subdirectory("external/assimp")

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC "include"
    PUBLIC "external/glad/include"
    PUBLIC "external/glfw/include"
    PUBLIC "external/freetype/include"
    PUBLIC "external/stb/include"
    PUBLIC "external/portaudio/include"
    PUBLIC "external/minimp3/include"
    PUBLIC "external/assimp/include"
)

# Linked libraries
target_link_libraries(${PROJECT_NAME} glfw freetype PortAudio assimp)

# Tests
option(MAYA_BUILD_TESTS "Build tests" ON)
if (MAYA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()