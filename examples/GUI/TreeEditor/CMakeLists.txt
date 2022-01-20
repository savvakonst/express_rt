

add_executable(ExampleTreeEditor
        main.cpp
        binder.cpp)

target_link_libraries(ExampleTreeEditor PRIVATE Qt5::Widgets)
target_link_libraries(ExampleTreeEditor PRIVATE CommonLib)
target_link_libraries(ExampleTreeEditor PRIVATE TreeEditor)
target_link_libraries(ExampleTreeEditor PUBLIC PyEmbedder)