
set(QT_PROPERTY_BROWSER QtPropertyBrowser)

file(GLOB Headers *.h)
file(GLOB Sources *.cpp)
file(GLOB Resources *.qrc)


add_library(${QT_PROPERTY_BROWSER}
        ${Sources}
        ${Headers}
        ${Resources}
        )

# TODO: remove it
target_include_directories(${QT_PROPERTY_BROWSER} PUBLIC .)


target_link_libraries(${QT_PROPERTY_BROWSER} PRIVATE Qt5::Widgets)

