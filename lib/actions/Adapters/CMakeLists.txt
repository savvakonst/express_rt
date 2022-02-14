include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_library(Adapters
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})

target_link_libraries(Adapters CommonLib)

if (WIN32)
    target_link_libraries(Adapters wsock32 ws2_32 iphlpapi)
endif ()


