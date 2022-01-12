include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_executable(ExampleModules
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set_property(TARGET ExampleModules PROPERTY OUTPUT_NAME ExampleModules)


target_link_libraries(ExampleModules CommonLib)
add_dependencies(ExampleModules Modules)

set_target_properties(ExampleModules PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME ExampleModules)
