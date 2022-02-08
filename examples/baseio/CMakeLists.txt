include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)


set(EXAMPLE_BASE_IO ExampleBaseIO)

add_library(${EXAMPLE_BASE_IO} SHARED
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


target_link_libraries(${EXAMPLE_BASE_IO} PRIVATE DefaultBaseIO)
target_link_libraries(${EXAMPLE_BASE_IO} PRIVATE KsdIO)
set_target_properties(${EXAMPLE_BASE_IO} PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME ${EXAMPLE_BASE_IO})
target_compile_definitions(${EXAMPLE_BASE_IO} PRIVATE DEFAULT_PARAMETERS_LIB_NAME=${EXAMPLE_BASE_IO})


