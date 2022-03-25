
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif ()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(TreeEditor)
add_subdirectory(ConnonViews)
add_subdirectory(qtpropertybrowser)
add_subdirectory(NewPlotter)