
if (MSVC)
    #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif ()

get_directory_property(hasParent PARENT_DIRECTORY)
if (NOT hasParent)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
endif ()


add_library(CommonLib SHARED
        common/main.cpp
        common/CustomTypes.cpp
        common/DataSchema_ifs.cpp
        common/ExtensionManager.cpp
        convtemplate/ConversionTemplate.cpp
        convtemplate/ParameterFieldTree.cpp
        convtemplate/StandardParameter_ifs.cpp
        convtemplate/ConversionTemplateManager.cpp
        device/Device_ifs.cpp
        device/TaskMapper.cpp
        device/KsdModule.cpp
        common/StringProcessingTools.cpp)


target_include_directories(CommonLib PUBLIC ../include)
target_compile_definitions(CommonLib PUBLIC DEBUG_)
target_compile_definitions(CommonLib PRIVATE COMMON_CREATE_SHARED_LIBRARY)
target_link_libraries(CommonLib Ati)

file(GLOB PNG_LIST ../png/modules/*.png)
file(COPY ${PNG_LIST} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/png/modules FOLLOW_SYMLINK_CHAIN)


add_subdirectory(IO)

add_subdirectory(modules)
add_subdirectory(bindings)


add_subdirectory("Reader")
add_subdirectory("ReaderExample")
add_subdirectory("Receiver")
add_subdirectory("Ping")
add_subdirectory("actions")
add_subdirectory("GUI")
add_subdirectory("DataStructures")