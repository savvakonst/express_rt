cmake_minimum_required(VERSION 3.10)

#project(BaseIO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)
#include_directories (../test_main/include)

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
