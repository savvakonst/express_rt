cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_CXX_FLAGS_RELEASE "-O2")


file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

#SHARED
add_library(ReaderExample
        ${Sources}
        ${Headers}
        )

target_include_directories(ReaderExample PUBLIC ..)

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})

set_property(TARGET ReaderExample PROPERTY OUTPUT_NAME ReaderExample)
target_link_libraries(ReaderExample Reader)


set_target_properties(ReaderExample PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME ReaderExample)
