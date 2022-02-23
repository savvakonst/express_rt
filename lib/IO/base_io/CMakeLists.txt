include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)


add_library(BaseIO
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set(YAML_BUILD_SHARED_LIBS ON)
set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_CPP_BUILD_TOOLS OFF)


include(FetchContent)
FetchContent_Declare( # TODO: need to set up a source code destination directory to avoid source code duplication
        yaml_cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG tags/yaml-cpp-0.6.3 #origin/master
        )

FetchContent_MakeAvailable(yaml_cpp)


target_link_libraries(BaseIO PRIVATE yaml-cpp)
target_link_libraries(BaseIO PUBLIC CommonLib)

set_target_properties(BaseIO PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME BaseIO)
#target_compile_definitions(BaseIO PRIVATE YAML_CPP_DLL)
#define YAML_CPP_DLL







