


add_library(TreeEditor
        TreeEditor.cpp
        TreeEditor.h)

# TODO: remove it
target_include_directories(TreeEditor PUBLIC .)

target_link_libraries(TreeEditor PRIVATE Qt5::Widgets)
target_link_libraries(TreeEditor PRIVATE CommonLib)