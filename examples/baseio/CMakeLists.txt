include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_executable(ExampleBaseIO
        ${Sources}
        ${Headers}
        EthernetUDP.cpp)

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})


set_property(TARGET ExampleBaseIO PROPERTY OUTPUT_NAME ExampleBaseIO)

target_link_libraries(ExampleBaseIO DefaultBaseIO)
#add_dependencies(ExampleBaseIO DefaultBaseIO)

