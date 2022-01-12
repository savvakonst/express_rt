include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)
add_library(DefaultBaseIO SHARED
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set_property(TARGET DefaultBaseIO PROPERTY OUTPUT_NAME DefaultBaseIO)

set(YAML_BUILD_SHARED_LIBS true)
add_subdirectory("../../../yaml-cpp" yaml-cpp_ EXCLUDE_FROM_ALL)

target_link_libraries(DefaultBaseIO yaml-cpp)
target_link_libraries(DefaultBaseIO CommonLib)

#target_compile_definitions(DefaultBaseIO PRIVATE yaml_cpp_EXPORTS)
#set_target_properties(DefaultBaseIO PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME DefaultBaseIO)






