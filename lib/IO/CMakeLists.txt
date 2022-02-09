include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)


set(COMMON_IO CommonIO)

add_library(${COMMON_IO} SHARED
        main.cpp
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


add_subdirectory(base_io)
add_subdirectory(base_io_extensions)
add_subdirectory(ksd_io)


target_link_libraries(${COMMON_IO} PRIVATE BaseIO)
target_link_libraries(${COMMON_IO} PRIVATE BaseIOExtensions)
target_link_libraries(${COMMON_IO} PRIVATE KsdIO)

set_target_properties(${COMMON_IO} PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME ${COMMON_IO})


target_compile_definitions(${COMMON_IO} PRIVATE COMMON_IO_LIB_NAME=${COMMON_IO})