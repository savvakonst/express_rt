

set(ACTIONS Actions)

add_library(${ACTIONS} SHARED
        main.cpp
        )


target_link_libraries(${ACTIONS} Ping)
target_link_libraries(${ACTIONS} Modules)


add_subdirectory(Adapters)
target_link_libraries(${ACTIONS} Adapters)

target_compile_definitions(${ACTIONS} PRIVATE ACTIONS_LIB_NAME=${ACTIONS})