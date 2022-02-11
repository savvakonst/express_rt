

set(ACTIONS Actions)

add_library(${ACTIONS} SHARED
        main.cpp
        )


target_link_libraries(${ACTIONS} Ping)
target_link_libraries(${ACTIONS} Modules)


target_compile_definitions(${ACTIONS} PRIVATE ACTIONS_LIB_NAME=${ACTIONS})