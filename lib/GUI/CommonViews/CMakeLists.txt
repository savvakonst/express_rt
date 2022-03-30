
set(COMMON_VIEWS CommonViews)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)

add_library(${COMMON_VIEWS} SHARED
        ${Headers}
        ${Sources}
        )

# TODO: remove it
target_include_directories(${COMMON_VIEWS} PRIVATE .)
#set_target_properties(${COMMON_VIEWS} PROPERTIES AUTOMOC TRUE)

#target_link_libraries(${COMMON_VIEWS} PUBLIC ksdExpressOnlineScreen)
target_link_libraries(${COMMON_VIEWS} PRIVATE Qt5::Widgets)
target_link_libraries(${COMMON_VIEWS} PRIVATE CommonLib)


target_compile_definitions(${COMMON_VIEWS} PRIVATE COMMON_VIEWS_LIB_NAME=${COMMON_VIEWS})