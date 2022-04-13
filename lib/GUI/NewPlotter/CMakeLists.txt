

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)


add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)
file(GLOB UiFiles *.ui)
file(GLOB QtResources *.qrc)


qt5_wrap_ui(UiHeaders ${UiFiles})
qt5_add_resources(QrcSources icons.qrc)

message(STATUS "________QtResources:${QrcSources}")
add_library(Plotter SHARED
        ${Sources}
        ${Headers}
        ${UiHeaders}
        ${QrcSources}
        )


target_include_directories(Plotter PUBLIC .)

target_link_libraries(Plotter QtPropertyBrowser)
target_link_libraries(Plotter ReaderExample)
target_link_libraries(Plotter Ping)
target_link_libraries(Plotter Modules)
target_link_libraries(Plotter Qt5::Widgets)


source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})
source_group(uis FILES ${UiFiles})
source_group(qrc FILES ${QtResources})


target_compile_definitions(Plotter PRIVATE PLOTTER_LIB_NAME=Plotter)

if (MSVC)
    set_target_properties(Plotter PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_PREFIX_PATH}\\bin")
endif ()
