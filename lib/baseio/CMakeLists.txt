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

add_subdirectory("../../../yaml-cpp" yaml-cpp_ EXCLUDE_FROM_ALL)
#message(------------------------------  ${PROJECT_SOURCE_DIR})


target_link_libraries(DefaultBaseIO yaml-cpp)
target_link_libraries(DefaultBaseIO CommonLib)

set_target_properties(DefaultBaseIO PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME DefaultBaseIO)






