include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_library(getAdaptersInfo
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})

if (WIN32)
    target_link_libraries(getAdaptersInfo wsock32 ws2_32 iphlpapi)
endif ()


