
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ExampleDockWindows
        main.cpp
        mainwindow.cpp
        dockwidgets.qrc)


target_link_libraries(ExampleDockWindows PUBLIC Qt5::Widgets)
target_link_libraries(ExampleDockWindows PUBLIC Qt5::PrintSupport)
#target_link_libraries(ExampleDockWindows PRIVATE CommonLib)
#target_link_libraries(ExampleDockWindows PRIVATE TreeEditor)
target_link_libraries(ExampleDockWindows PUBLIC PyEmbedder)

if (MSVC)
    set_target_properties(ExampleDockWindows PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_PREFIX_PATH}\\bin")
endif ()