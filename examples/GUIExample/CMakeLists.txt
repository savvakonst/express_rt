cmake_minimum_required(VERSION 3.1.0)

project(helloworld VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)


add_executable(GUIExample
        main.cpp
        MainWindow.cpp
        MainWindow.h)


target_link_libraries(GUIExample Ping)
target_link_libraries(GUIExample ReaderExample)
target_link_libraries(GUIExample Qt5::Widgets)


if (MSVC)
    set_target_properties(GUIExample PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_PREFIX_PATH}\\bin")
endif ()