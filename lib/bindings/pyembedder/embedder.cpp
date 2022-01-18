#ifndef PY_EMBEDDER_LIB_NAME
#    error "PY_EMBEDDER_LIB_NAME undefined"
#endif

#include <pybind11/embed.h>

#include <iostream>

#include "common/Extension.h"
#include "pybinder/binder.h"

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

typedef std::vector<DataSchema_ifs *> bindList_t;

static ExtensionUnit *g_pyembedder_units = nullptr;

static ExtensionInfo g_pyembedder_info = {"python bindings", 0x01, g_pyembedder_units};

static py::scoped_interpreter *g_guard = nullptr;

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, PY_EMBEDDER_LIB_NAME)(void) {
    g_guard = new py::scoped_interpreter{};
    py::module_ bind = py::module_::import("expressbind");
    bindList_t ds_list;
    {
        py::list py_cat = bind.attr("loadConfig")();
        for (auto i : py_cat) {
            try {
                PyDataSchema *cpp_ds = i.cast<PyDataSchema *>();
                cpp_ds->setOwnerShip(i);
                i.inc_ref();

                ds_list.push_back(cpp_ds);

            } catch (const std::runtime_error &e) {
                std::cout << e.what();
            }
        }
    }

    g_pyembedder_units = new ExtensionUnit[ds_list.size() + 1];
    auto tmp_units = g_pyembedder_units;
    for (auto i : ds_list) {  //{"CH04", "module", "CH04 ksd module", (void *)&createCH04Module, 0x00},
        *(tmp_units++) = {i->name_.c_str(), "data_schema", i->description_.c_str(), (void *)i, 0x00};
    }
    *(tmp_units) = {nullptr, nullptr, nullptr, nullptr, 0};
    g_pyembedder_info.units = g_pyembedder_units;
    return &g_pyembedder_info;
}

InitExtension(int) POST_CONCATENATOR(release, PY_EMBEDDER_LIB_NAME)(void) {
    if (g_pyembedder_units) delete[] g_pyembedder_units;

    delete g_guard;

    return 0;
}
