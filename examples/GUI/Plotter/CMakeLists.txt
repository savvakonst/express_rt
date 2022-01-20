find_package(Qt5 COMPONENTS Widgets REQUIRED)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ExamplePlotter
        main.cpp
        TopWindow.cpp
        TopWindow.h)


target_link_libraries(ExamplePlotter Ping)
target_link_libraries(ExamplePlotter ReaderExample)
target_link_libraries(ExamplePlotter Modules)
target_link_libraries(ExamplePlotter Qt5::Widgets)


if (MSVC)
    set_target_properties(ExamplePlotter PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_PREFIX_PATH}\\bin")
endif ()