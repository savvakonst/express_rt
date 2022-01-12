

######### init pybind11_DIR ###########
execute_process(COMMAND pybind11-config --cmakedir OUTPUT_VARIABLE pybind11_DIR)
string(REGEX REPLACE "\n$" "" pybind11_DIR "${pybind11_DIR}")

find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)

#######################################
add_subdirectory(pybinder)
add_subdirectory(pyembedder)
#add_subdirectory(../../examples/bindings) # TODO: it's dirty hack