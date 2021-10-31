cmake_minimum_required(VERSION 3.10)

project(Common)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif ()


add_library(CommonLib SHARED
        lib/convtemplate/ConversionTemplate.cpp
        lib/device/Device.cpp
        examples/modules/main.cpp)

target_include_directories(CommonLib PUBLIC include)
target_compile_definitions(CommonLib PRIVATE COMMON_CREATE_SHARED_LIBRARY)


add_subdirectory(lib/baseio)
add_subdirectory(lib/modules)

if (MSVC)
    target_compile_options(CommonLib PUBLIC /wd4251)
endif ()




