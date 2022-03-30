cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(READER Reader)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_library(${READER} SHARED
        ${Sources}
        ${Headers}
        )

target_include_directories(${READER} PUBLIC ..)

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})

target_link_libraries(${READER} CommonLib)


set_property(TARGET ${READER} PROPERTY OUTPUT_NAME ${READER})


set_target_properties(${READER} PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME ${READER})
target_compile_definitions(${READER} PRIVATE READER_LIB_NAME=${READER})