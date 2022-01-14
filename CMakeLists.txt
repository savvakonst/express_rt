cmake_minimum_required(VERSION 3.10)

project(EXRT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_directory_property(hasParent PARENT_DIRECTORY)
if (NOT hasParent)
    set(VENV_PATH bin)

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${VENV_PATH})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${VENV_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${VENV_PATH})
endif ()


add_subdirectory(lib)
add_subdirectory(examples)


if (MSVC)
    target_compile_options(CommonLib PUBLIC /wd4251)
endif ()




