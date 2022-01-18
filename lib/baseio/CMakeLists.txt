include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)


add_library(DefaultBaseIO
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set(YAML_BUILD_SHARED_LIBS true)
add_subdirectory("../../../yaml-cpp" yaml-cpp)


target_link_libraries(DefaultBaseIO PRIVATE yaml-cpp)
target_link_libraries(DefaultBaseIO PUBLIC CommonLib)

#target_compile_definitions(DefaultBaseIO PRIVATE yaml_cpp_EXPORTS)
set_target_properties(DefaultBaseIO PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME DefaultBaseIO)








