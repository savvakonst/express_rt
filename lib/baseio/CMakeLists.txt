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

include(FetchContent)
FetchContent_Declare( # TODO: need to set up a source code destination directory to avoid source code duplication
        yaml_cpp
        GIT_REPOSITORY    https://github.com/jbeder/yaml-cpp.git
        GIT_TAG           origin/master #/yaml-cpp-0.7.0
)
FetchContent_MakeAvailable(yaml_cpp)


target_link_libraries(DefaultBaseIO PRIVATE yaml-cpp)
target_link_libraries(DefaultBaseIO PUBLIC CommonLib)

set_target_properties(DefaultBaseIO PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME DefaultBaseIO)








