cmake_minimum_required(VERSION 3.20)

project(Maya)

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

add_library(${PROJECT_NAME} "src/core.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_subdirectory("3rdparty/glfw")

target_include_directories(${PROJECT_NAME}
    PUBLIC "include"
    PUBLIC "3rdparty/glfw/include"
)

target_link_libraries(${PROJECT_NAME}
    glfw
)

option(MAYA_BUILD_TEST "Build test" ON)
if (MAYA_BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()