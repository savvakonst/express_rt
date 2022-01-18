include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_library(ExampleBaseIO SHARED
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


target_link_libraries(ExampleBaseIO PRIVATE DefaultBaseIO)
set_target_properties(ExampleBaseIO PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME ExampleBaseIO)
target_compile_definitions(ExampleBaseIO PRIVATE DEFAULT_PARAMETERS_LIB_NAME=ExampleBaseIO)


