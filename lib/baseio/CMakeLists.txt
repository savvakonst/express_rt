include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)


add_library(DefaultBaseIO
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set(YAML_BUILD_SHARED_LIBS ON)
add_subdirectory("../../../yaml-cpp" yaml-cpp)


target_link_libraries(DefaultBaseIO PRIVATE yaml-cpp)
target_link_libraries(DefaultBaseIO PUBLIC CommonLib)

set_target_properties(DefaultBaseIO PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME DefaultBaseIO)








