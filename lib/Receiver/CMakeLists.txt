cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

#SHARED
add_library(Receiver
        ${Sources}
        ${Headers})

target_include_directories(Receiver PUBLIC ..)

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})

set_property(TARGET Receiver PROPERTY OUTPUT_NAME Receiver)
target_link_libraries(Receiver CommonLib)

if (WIN32)
    target_link_libraries(Receiver wsock32 ws2_32)
endif ()

set_target_properties(Receiver PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME Receiver)
