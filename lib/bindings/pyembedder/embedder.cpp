//
// Created by SVK on 02.12.2021.
//

#include <pybind11/embed.h>

#include <iostream>

#include "../pybinder/binder.h"

namespace py = pybind11;

DataSchema_ifs *newDataSchema(py::module_ &bind, const std::string &name) {
    PyDataSchema *cpp_cat = nullptr;
    try {
        py::object py_cat = bind.attr("DataSchemaInst")(name);
        cpp_cat = py_cat.cast<PyDataSchema *>();
        py::handle handle = py_cat.release();
        cpp_cat->setOwnerShip(handle);
    } catch (const std::runtime_error &e) {
        std::cout << e.what();
    }
    return cpp_cat;
}

/*
int main() {
    py::scoped_interpreter guard{};

    py::module_ bind = py::module_::import("expressbind");

    DataSchema_ifs *cpp_cat = newDataSchema(bind, "name");

    if (cpp_cat) {
        delete cpp_cat;
    }

    return 0;
}*/