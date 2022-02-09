
set(CONV_TEMPLATE_LIST ConvTemplateList)


add_library(${CONV_TEMPLATE_LIST} SHARED
        main.cpp
        ConvTemplateList.cpp
        ConvTemplateList.h
        ParameterTable.cpp
        ParameterTable.h
        DeviceList.cpp
        DeviceList.h
        Actions.h)

# TODO: remove it
target_include_directories(${CONV_TEMPLATE_LIST} PRIVATE .)
#set_target_properties(${CONV_TEMPLATE_LIST} PROPERTIES AUTOMOC TRUE)


target_link_libraries(${CONV_TEMPLATE_LIST} PRIVATE Qt5::Widgets)
target_link_libraries(${CONV_TEMPLATE_LIST} PRIVATE CommonLib)
target_compile_definitions(${CONV_TEMPLATE_LIST} PRIVATE CONV_TEMPLATE_LIST_LIB_NAME=${CONV_TEMPLATE_LIST})