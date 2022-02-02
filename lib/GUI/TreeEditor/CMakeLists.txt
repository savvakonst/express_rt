
set(TREE_EDITOR TreeEditor)

add_library(${TREE_EDITOR} SHARED
        TreeEditor.cpp
        ../../../include/GUI/TreeEditor.h
        TreeTextEdit.h)

# TODO: remove it
target_include_directories(${TREE_EDITOR} PRIVATE .)
#set_target_properties(${TREE_EDITOR} PROPERTIES AUTOMOC TRUE)


target_link_libraries(${TREE_EDITOR} PRIVATE Qt5::Widgets)
target_link_libraries(${TREE_EDITOR} PRIVATE CommonLib)
target_compile_definitions(${TREE_EDITOR} PRIVATE TREE_EDITOR_LIB_NAME=${TREE_EDITOR})