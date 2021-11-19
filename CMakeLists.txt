cmake_minimum_required(VERSION 3.10)

project(EXRT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_directory_property(hasParent PARENT_DIRECTORY)
if (hasParent)
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
endif ()

add_subdirectory(lib)
add_subdirectory(examples)


if (MSVC)
    target_compile_options(CommonLib PUBLIC /wd4251)
endif ()




