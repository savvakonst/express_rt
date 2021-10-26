cmake_minimum_required (VERSION 3.10)

project(Common)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif()



add_library(ConversionTemplate SHARED
        src/convtemplate/ConversionTemplate.cpp
        src/device/Device.cpp
        src/modules/A01_Module.cpp include/device/KsdModule.h)

target_include_directories(ConversionTemplate PUBLIC include )
target_compile_definitions(ConversionTemplate PRIVATE COMMON_CREATE_SHARED_LIBRARY)


add_subdirectory(src/baseio)

if(MSVC)
    target_compile_options(ConversionTemplate PUBLIC  /wd4251)
endif()




