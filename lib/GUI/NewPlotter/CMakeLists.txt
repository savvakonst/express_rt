cmake_minimum_required(VERSION 3.1.0)

project(ksdExpressOnlineScreen VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif ()


find_package(Qt5 COMPONENTS Widgets REQUIRED)


add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)
file(GLOB UiFiles *.ui)
file(GLOB QtResources *.qrc)


qt5_wrap_ui(UiHeaders ${UiFiles})


add_library(ksdExpressOnlineScreen
        ${Sources}
        ${Headers}
        ${UiHeaders}
        ${QtResources}
        )

target_include_directories(ksdExpressOnlineScreen PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(ksdExpressOnlineScreen PUBLIC .)

target_link_libraries(ksdExpressOnlineScreen QtPropertyBrowser)
target_link_libraries(ksdExpressOnlineScreen ReaderExample)
target_link_libraries(ksdExpressOnlineScreen Ping)
target_link_libraries(ksdExpressOnlineScreen Modules)
target_link_libraries(ksdExpressOnlineScreen Qt5::Widgets)


source_group(src FILES ${Sources})
source_group(headers FILES ${Headers})
source_group(uis FILES ${UiFiles})
source_group(qrc FILES ${QtResources})


if (MSVC)
    set_target_properties(ksdExpressOnlineScreen PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_PREFIX_PATH}\\bin")
endif ()
