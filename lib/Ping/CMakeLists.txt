cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)


find_package(Qt5 COMPONENTS Widgets REQUIRED)
#SHARED
add_library(Ping
        ${Sources}
        ${Headers})


target_include_directories(Ping PUBLIC ..)

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})

set_property(TARGET Ping PROPERTY OUTPUT_NAME Ping)

target_link_libraries(Ping Reader)
target_link_libraries(Ping Receiver)
target_link_libraries(Ping Modules)
target_link_libraries(Ping CommonLib)
target_link_libraries(Ping Qt5::Widgets)

if (WIN32)
    target_link_libraries(Ping wsock32 ws2_32)
endif ()

set_target_properties(Ping PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME Ping)
