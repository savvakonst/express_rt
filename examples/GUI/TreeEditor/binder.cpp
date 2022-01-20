//
// Created by SVK on 30.12.2021.
//

//#include "common/../../lib/bindings//pybinder/binder.h"

#include "pybinder/binder.h"

#include <pybind11/embed.h>

namespace py = pybind11;

typedef std::vector<DataSchema_ifs *> bindList_t;

// py::scoped_interpreter guard{};
// py::module_ bind = py::module_::import("expressbind");

std::string printer_debug(DataSchema_ifs *arg) { return toString(arg, ""); }

bindList_t getConfig() {
    py::scoped_interpreter *guard = new py::scoped_interpreter{};
    py::module_ bind = py::module_::import("expressbind");
    bindList_t ds_list;

    {
        py::list py_cat = bind.attr("loadConfig")();
        for (auto i : py_cat) {
            try {
                PyDataSchema *cpp_ds = i.cast<PyDataSchema *>();
                cpp_ds->setOwnerShip(i);
                i.inc_ref();
                std::cout << "\n\nprinter_debug:\n" << printer_debug(cpp_ds);
                ;
                ds_list.push_back(cpp_ds);

            } catch (const std::runtime_error &e) {
                std::cout << e.what();
            }
        }
    }
    return ds_list;
}