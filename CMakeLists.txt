cmake_minimum_required(VERSION 3.10)

project(EXRT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_directory_property(hasParent PARENT_DIRECTORY)
if (NOT hasParent)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
endif ()



execute_process(COMMAND pybind11-config --cmakedir OUTPUT_VARIABLE pybind11_DIR)
string(REGEX REPLACE "\n$" "" pybind11_DIR "${pybind11_DIR}")
message(STATUS "pybind11_DIR: " ${pybind11_DIR})


find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)
message(STATUS "pybind11_INCLUDE_DIRS: " ${pybind11_INCLUDE_DIRS})


add_subdirectory(lib)
add_subdirectory(examples)



if (MSVC)
    target_compile_options(CommonLib PUBLIC /wd4251)
endif ()




