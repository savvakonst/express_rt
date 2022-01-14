
set(PYEMBEDDER_NAME PyEmbedder)

add_library(${PYEMBEDDER_NAME} SHARED
        embedder.cpp
        )

add_dependencies(${PYEMBEDDER_NAME} pybinder)


target_include_directories(${PYEMBEDDER_NAME} PUBLIC ../../../../pybindings/lib)
target_link_libraries(${PYEMBEDDER_NAME} PUBLIC pybind11::embed)
target_link_libraries(${PYEMBEDDER_NAME} PRIVATE CommonLib)

target_compile_definitions(${PYEMBEDDER_NAME} PRIVATE PY_EMBEDDER_LIB_NAME=${PYEMBEDDER_NAME})