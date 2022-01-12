
#include <pybind11/embed.h>
#include <iostream>
#include "pybinder/binder.h"

namespace py = pybind11;

DataSchema_ifs *newDataSchema(py::module_ &bind, const std::string &name) {
    PyDataSchema *cpp_ds = nullptr;
    try {
        py::object py_ds = bind.attr("DataSchemaInst")(name);
        cpp_ds = py_ds.cast<PyDataSchema *>();
        py::handle handle = py_ds.release();
        cpp_ds->setOwnerShip(handle);
    } catch (const std::runtime_error &e) {
        std::cout << e.what();
    }
    return cpp_ds;
}

std::string printer_debug(DataSchema_ifs *arg) {
    return indentationPrinter_debug(arg, "");
}

int main() {
    py::scoped_interpreter guard{};

    py::module_ bind = py::module_::import("expressbind");

    //DataSchema_ifs *cpp_cat = newDataSchema(bind, "name");
    //if (cpp_cat) {
    //    delete cpp_cat;
    //}
    PyDataSchema *cpp_ds = nullptr;
    {
        py::list py_cat = bind.attr("loadConfig")();
        for (auto i: py_cat) {

            try {

                cpp_ds = i.cast<PyDataSchema *>();
                cpp_ds->setOwnerShip(i);
                i.inc_ref();
                std::cout << "\n\nprinter_debug:\n" << printer_debug(cpp_ds);;

            } catch (const std::runtime_error &e) {
                std::cout << e.what();
            }
        }
    }


    std::cout << "\n\nend:\n";

    if (cpp_ds) {
        delete cpp_ds;
    }

    //py::list py_list = py_cat;


    return 0;
}