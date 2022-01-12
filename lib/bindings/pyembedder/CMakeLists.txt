

add_library(pyembedder
        embedder.cpp
        )

add_dependencies(pyembedder pybinder)


target_include_directories(pyembedder PUBLIC ../../../../pybindings/lib)
target_link_libraries(pyembedder PUBLIC pybind11::embed)
target_link_libraries(pyembedder PRIVATE CommonLib)