#ifndef PY_BINDLIB_NAME
#    define PY_BINDLIB_NAME test_pybind
#endif

#include "binder.h"

#include <iostream>
#include <memory>

/*
 *
 *
 */

std::string printer_debug(DataSchema_ifs *arg) { return toString(arg, ""); }

DataSchema_ifs *getDataSchema(ExtensionManager *manager, const std::string &name) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", name);
    if (unit == nullptr || unit->ptr == nullptr) return nullptr;
    auto data_schema = (DataSchema_ifs *)unit->ptr;
    return data_schema;
}

PYBIND11_MODULE(PY_BINDLIB_NAME, m) {
    m.doc() = "exrt bindings";

    // https://pybind11.readthedocs.io/en/stable/advanced/functions.html

    py::class_<ExtensionUnit, std::unique_ptr<ExtensionUnit, py::nodelete>>(m, "ExtensionUnit");

    py::class_<ExtensionManager>(m, "ExtensionManager")
        .def(py::init([]() { return new ExtensionManager; }))
        .def("getAvailableExtensionTypes", &ExtensionManager::getAvailableExtensionTypes)
        .def("getLastVersionExtensionUintsByType", &ExtensionManager::getLastVersionExtensionUintsByType)
        .def("getExtensionUintSet", &ExtensionManager::getExtensionUintSet)
        .def("getLastVersionExtensionUint", &ExtensionManager::getLastVersionExtensionUint);

    py::class_<Value>(m, "Value")
        .def(py::init([]() { return new Value(); }))
        .def(py::init([](const Value &a) { return new Value(a); }))
        .def(py::init([](bool a) { return new Value(a); }))
        .def(py::init([](double a) { return new Value(a); }))
        .def(py::init([](int64_t a) { return new Value(a); }))
        .def(py::init<const std::string &>())
        .def(py::init<const std::string &, const std::string &>())
        .def("asString", &Value::asString)
        .def("__repr__", &Value::asString)
        .def("__bool__", &Value::operator bool);

    py::class_<DataSchema_ifs, PyDataSchema, std::unique_ptr<DataSchema_ifs, py::nodelete>>(m, "DataSchema")
        .def(py::init<const std::string &, const std::string &>())
        .def("init", &DataSchema_ifs::init)
        .def("getType", &DataSchema_ifs::getType)
        .def("getRepresentationType", &DataSchema_ifs::getRepresentationType)
        .def("isValid", &DataSchema_ifs::isValid)
        .def("isMap", &DataSchema_ifs::isMap)
        .def("isArray", &DataSchema_ifs::isArray)
        .def("getDims", &DataSchema_ifs::getDims)
        .def("getMapList", &DataSchema_ifs::getMapList)
        .def("validate", &DataSchema_ifs::validate)
        .def_readwrite("name_", &DataSchema_ifs::name_)
        .def_readwrite("description_", &DataSchema_ifs::description_)
        .def_readwrite("help_", &DataSchema_ifs::help_)
        .def_readwrite("default_value_", &DataSchema_ifs::default_value_)
        .def_readwrite("enums_", &DataSchema_ifs::enums_)
        .def("__del__", [](DataSchema_ifs *ds) {
            if (!((PyDataSchema *)ds)->handle_) {
                delete ((PyDataSchema *)ds);
            }
        });

    py::class_<HierarchicalData_ifs, PyHierarchicalData>(m, "HierarchicalData")
        .def(py::init<>())
        .def("isArray", &HierarchicalData_ifs::isArray)
        .def("isMap", &HierarchicalData_ifs::isMap)
        .def("isValue", &HierarchicalData_ifs::isValue)
        .def("getValue", &HierarchicalData_ifs::getValue)
        .def("getArray", &HierarchicalData_ifs::getArray)
        .def("getMap", &HierarchicalData_ifs::getMap)
        .def("getArrayUint", &HierarchicalData_ifs::getArrayUint)
        .def("getMapUint", &HierarchicalData_ifs::getMapUint);

    m.def("isNum", [](std::string a) { return isNum(createDataType(a)); });
    m.def("normalizeType", [](std::string a) { return toString(createDataType(a)); });
    m.def("printer_debug", printer_debug);
    m.def("getDataSchema", getDataSchema);
}
