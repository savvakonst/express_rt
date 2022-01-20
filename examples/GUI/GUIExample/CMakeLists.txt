
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