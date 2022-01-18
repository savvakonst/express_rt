include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_executable(ExampleLinker
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set_property(TARGET ExampleLinker PROPERTY OUTPUT_NAME ExampleLinker)


target_link_libraries(ExampleLinker CommonLib)
add_dependencies(ExampleLinker Modules)
add_dependencies(ExampleLinker PyEmbedder)
add_dependencies(ExampleLinker ExampleBaseIO)

set_target_properties(ExampleLinker PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME ExampleLinker)
