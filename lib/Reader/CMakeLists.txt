cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_CXX_FLAGS_RELEASE "-O2")

#include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_library(Reader
        ${Sources}
        ${Headers}
        )

target_include_directories(Reader PUBLIC ..)

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set_property(TARGET Reader PROPERTY OUTPUT_NAME Reader)

#add_subdirectory("../../express_rt" CommonLib EXCLUDE_FROM_ALL)
target_link_libraries(Reader CommonLib)


set_target_properties(Reader PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME Reader)
