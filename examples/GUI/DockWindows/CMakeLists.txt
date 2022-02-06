
find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


#  set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION C:/Express_expr_compiler/nik/express_rt)
set_source_files_properties(dictionary_ru.ts PROPERTIES OUTPUT_LOCATION ${PROJECT_SOURCE_DIR})
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} dictionary_ru.ts)


add_executable(ExampleDockWindows
        main.cpp
        mainwindow.cpp
        ${QM_FILES}
        )


message(STATUS "QM_FILES ${PROJECT_SOURCE_DIR}")


message(STATUS "QM_FILES ${QM_FILES}")

target_link_libraries(ExampleDockWindows PUBLIC Qt5::Widgets)
target_link_libraries(ExampleDockWindows PUBLIC Qt5::PrintSupport)
target_link_libraries(ExampleDockWindows PRIVATE CommonLib)
#target_link_libraries(ExampleDockWindows PRIVATE TreeEditor)
add_dependencies(ExampleDockWindows Modules)
add_dependencies(ExampleDockWindows PyEmbedder)
add_dependencies(ExampleDockWindows ExampleBaseIO)
add_dependencies(ExampleDockWindows ConvTemplateList)
add_dependencies(ExampleDockWindows TreeEditor)


if (MSVC)
    set_target_properties(ExampleDockWindows PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_PREFIX_PATH}\\bin")
endif ()