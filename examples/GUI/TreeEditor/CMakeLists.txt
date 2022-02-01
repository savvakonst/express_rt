
find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(ExampleTreeEditor
        main.cpp
        #binder.cpp
        )

target_link_libraries(ExampleTreeEditor PRIVATE Qt5::Widgets)
target_link_libraries(ExampleTreeEditor PRIVATE CommonLib)
target_link_libraries(ExampleTreeEditor PRIVATE TreeEditor)
target_link_libraries(ExampleTreeEditor PUBLIC PyEmbedder)

if (MSVC)
    set_target_properties(ExampleTreeEditor PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_PREFIX_PATH}\\bin")
endif ()