cmake_minimum_required (VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories (.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)
add_library(Modules
        ${Sources}
        ${Headers}
        )

source_group(src       FILES ${Sources})
source_group(headers   FILES ${Headers})


set_property(TARGET Modules PROPERTY OUTPUT_NAME Modules)


target_link_libraries(Modules CommonLib)

set_target_properties(Modules PROPERTIES CXX_STANDARD_REQUIRED ON OUTPUT_NAME Modules)