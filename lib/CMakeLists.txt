cmake_minimum_required(VERSION 3.10)

#project(Common)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif ()

get_directory_property(hasParent PARENT_DIRECTORY)
if (NOT hasParent)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
endif ()


add_library(CommonLib SHARED
        convtemplate/ConversionTemplate.cpp
        device/Device.cpp
        )


target_include_directories(CommonLib PUBLIC ../include)
target_compile_definitions(CommonLib PRIVATE COMMON_CREATE_SHARED_LIBRARY)


add_subdirectory(baseio)
add_subdirectory(modules)
