include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)


set(BASE_IO_EXTENSIONS BaseIOExtensions)

add_library(${BASE_IO_EXTENSIONS}
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


target_link_libraries(${BASE_IO_EXTENSIONS} PRIVATE BaseIO)
target_link_libraries(${BASE_IO_EXTENSIONS} PRIVATE KsdIO)
set_target_properties(${BASE_IO_EXTENSIONS} PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME ${BASE_IO_EXTENSIONS})
target_compile_definitions(${BASE_IO_EXTENSIONS} PRIVATE DEFAULT_PARAMETERS_LIB_NAME=${BASE_IO_EXTENSIONS})


