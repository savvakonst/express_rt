
set(PYBINDER_NAME pybinder)

pybind11_add_module(${PYBINDER_NAME} SHARED
        binder.cpp
        binder.h
        )


#target_include_directories(${PYBINDER_NAME} PUBLIC .)
target_link_libraries(${PYBINDER_NAME} PRIVATE CommonLib)
target_compile_definitions(${PYBINDER_NAME} PRIVATE PY_BINDLIB_NAME=${PYBINDER_NAME})