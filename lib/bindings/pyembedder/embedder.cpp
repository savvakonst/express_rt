#ifndef PY_EMBEDDER_LIB_NAME
#    error "PY_EMBEDDER_LIB_NAME undefined"
#endif

#include <pybind11/embed.h>

#include <iostream>

#include "common/Extension.h"
#include "pybinder/binder.h"

namespace py = pybind11;
using namespace py::literals;

typedef std::vector<DataSchema_ifs *> bindList_t;

static ExtensionUnit *g_pyembedder_units = nullptr;
static ExtensionInfo g_pyembedder_info = {"python bindings", 0x01, g_pyembedder_units};
static py::scoped_interpreter *g_guard = nullptr;

static ExtensionManager *g_extension_manager = nullptr;

PYBIND11_EMBEDDED_MODULE(pyexrt, m) {
    m.def(
        "getExtensionManager", []() { return g_extension_manager; },  //
        py::return_value_policy::reference);
}

static int initRyexrt(ExtensionManager *extension_manager) {
    g_extension_manager = extension_manager;
    return 0;
}

static int pyEvalFile(const std::string &path) {
    try {
        py::eval_file(path);
    } catch (const std::runtime_error &e) {
        std::cout << e.what();
    }
    return 0;
}

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, PY_EMBEDDER_LIB_NAME)(void) {
    g_guard = new py::scoped_interpreter{};
    bindList_t ds_list;
    try {
        py::module_ bind = py::module_::import("expressbind");

        {
            py::list py_cat = bind.attr("loadConfig")();
            for (auto i : py_cat) {
                auto *cpp_ds = i.cast<PyDataSchema *>();
                cpp_ds->setOwnerShip(i);
                i.inc_ref();

                ds_list.push_back(cpp_ds);
            }
        }
        auto kwargs = py::dict("name"_a = "World", "number"_a = 42);
        // auto kwargs = py::dict("\"name\"_a=\"World\"", "\"number\"_a=42");
        auto message = "Hello, {name}! The answer is {number}"_s.format(**kwargs);
        py::print(message);

    } catch (const std::runtime_error &e) {
        std::cout << e.what();
    }
    g_pyembedder_units = new ExtensionUnit[ds_list.size() + 3];
    auto tmp_units = g_pyembedder_units;
    for (auto i : ds_list) {  //{"CH04", "module", "CH04 ksd module", (void *)&createCH04Module, 0x00},
        *(tmp_units++) = {i->name_.c_str(), "data_schema", i->description_.c_str(), (void *)i, 0x00};
    }
    *(tmp_units++) = {
        "pyexrt", "init",
        "init pyexrt module. After that  getExtensionManager will return non zero pointer for current ExtensionManager",
        (void *)initRyexrt, 0x00};
    *(tmp_units++) = {"py_eval_file", "eval_file", "", (void *)pyEvalFile, 0x00};
    *(tmp_units) = {nullptr, nullptr, nullptr, nullptr, 0};
    g_pyembedder_info.units = g_pyembedder_units;
    return &g_pyembedder_info;
}

InitExtension(int) POST_CONCATENATOR(release, PY_EMBEDDER_LIB_NAME)(void) {
    if (g_pyembedder_units) delete[] g_pyembedder_units;

    delete g_guard;

    return 0;
}
