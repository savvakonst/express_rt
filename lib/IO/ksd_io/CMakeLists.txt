include_directories(.)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)


add_library(KsdIO
        ${Sources}
        ${Headers}
        )

source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})

target_link_libraries(KsdIO PUBLIC CommonLib)