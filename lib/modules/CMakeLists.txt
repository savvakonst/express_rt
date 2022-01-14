include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)
add_library(Modules SHARED
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set_property(TARGET Modules PROPERTY OUTPUT_NAME Modules)

target_link_libraries(Modules Reader)
target_link_libraries(Modules CommonLib)

set_target_properties(Modules PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME Modules)