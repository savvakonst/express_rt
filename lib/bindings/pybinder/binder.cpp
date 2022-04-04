#ifndef PY_BINDLIB_NAME
#    error "PY_BINDLIB_NAME undefined"
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
    auto unit = manager->getLastVersionExtensionUnit("data_schema", name);
    if (unit == nullptr || unit->ptr == nullptr) return nullptr;
    auto data_schema = (DataSchema_ifs *)unit->ptr;
    return data_schema;
}

DeviceViewWrapper_ifs *getDeviceViewWrapper(ExtensionManager *manager) {
    return (DeviceViewWrapper_ifs *)manager->getLastVersionExtensionObject("widget_wrapper", "device_view_wrapper");
}

ParameterViewWrapper_ifs *getParameterViewWrapper(ExtensionManager *manager) {
    return (ParameterViewWrapper_ifs *)manager->getLastVersionExtensionObject("widget_wrapper",
                                                                              "parameter_view_wrapper");
}

ExrtAction_ifs *getExrtAction(ExtensionManager *manager, const std::string &name) {
    auto unit = manager->getLastVersionExtensionUnit("exrt_action", name);
    if (unit == nullptr || unit->ptr == nullptr) return nullptr;
    auto data_schema = (ExrtAction_ifs *)unit->ptr;
    return data_schema;
}

IO_ifs *getPyIO(ExtensionManager *manager, const std::string &name) {
    auto unit = manager->getLastVersionExtensionUnit("io", name);
    if (unit == nullptr || unit->ptr == nullptr) return nullptr;
    auto io = (IO_ifs *)unit->ptr;
    return io;
}

PYBIND11_MODULE(PY_BINDLIB_NAME, m) {
    m.doc() = "exrt bindings";

    // https://pybind11.readthedocs.io/en/stable/advanced/functions.html

    py::class_<ExtensionUnit, std::unique_ptr<ExtensionUnit, py::nodelete>>(m, "ExtensionUnit");

    py::class_<ExtensionManager>(m, "ExtensionManager")
        .def(py::init([]() { return new ExtensionManager; }))
        .def("getAvailableExtensionTypes", &ExtensionManager::getAvailableExtensionTypes)
        .def("getLastVersionExtensionUnitsByType", &ExtensionManager::getLastVersionExtensionUnitsByType)
        .def("getExtensionUnitSet", &ExtensionManager::getExtensionUnitSet)
        .def("getLastVersionExtensionUnit", &ExtensionManager::getLastVersionExtensionUnit);

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
        .def("getArrayUnit", &HierarchicalData_ifs::getArrayUnit)
        .def("getMapUnit", &HierarchicalData_ifs::getMapUnit);

    py::class_<Module_ifs, PyModule>(m, "Module")
        .def(py::init<>())
        .def("hasTransceiver", &Module_ifs::hasTransceiver)
        .def("getSrcAddress", &Module_ifs::getSrcAddress)
        .def("isAvailable", &Module_ifs::isAvailable)
        .def("getID", &Module_ifs::getID)
        .def("getModulePath", &Module_ifs::getModulePath)
        .def("getParentModule", &Module_ifs::getParentModule, py::return_value_policy::reference)
        .def("setParentModule", &Module_ifs::setParentModule)
        .def("getSubModulesFromPath", &Module_ifs::getSubModulesFromPath, py::return_value_policy::reference)
        .def("getPrmBufferMap", &Module_ifs::getPrmBufferMap, py::return_value_policy::reference)
        .def("getPropertySchema", &Module_ifs::getPropertySchema, py::return_value_policy::reference)
        .def("printProperties", &Module_ifs::printProperties)
        .def("getProperty", &Module_ifs::getProperty, py::return_value_policy::reference)
        .def("getPropertyAsTxt", &Module_ifs::getPropertyAsTxt)
        .def<bool (Module_ifs::*)(const std::string &, const Value &)>("setProperty", &Module_ifs::setProperty)
        .def<bool (Module_ifs::*)(const std::string &, const HierarchicalData_ifs *)>("setProperty",
                                                                                      &Module_ifs::setProperty)
        .def("setPropertyAsTxt", &Module_ifs::setPropertyAsTxt)
        .def("isChannelAvailable", &Module_ifs::isChannelAvailable)
        .def("getFrequency", &Module_ifs::getChannelProperty)
        .def("storeTaskToBuffer", &Module_ifs::storeTaskToBuffer)
        .def("getTaskSize", &Module_ifs::getTaskSize)
        .def("getSubModules", &Module_ifs::getSubModules, py::return_value_policy::reference)
        .def("createModuleStream", &Module_ifs::createModuleStream);

    py::class_<Parameter_ifs, PyParameter>(m, "Parameter")
        .def("createBuffer", &Parameter_ifs::createBuffer)
        .def("getType", &Parameter_ifs::getType)
        .def("getPropertySchema", &Parameter_ifs::getPropertySchema, py::return_value_policy::reference)
        .def("getProperty", &Parameter_ifs::getProperty, py::return_value_policy::reference)
        .def("getPropertyAsTxt", &Parameter_ifs::getPropertyAsTxt)
        .def<bool (Parameter_ifs::*)(const std::string &, const HierarchicalData_ifs *)>("setProperty",
                                                                                         &Parameter_ifs::setProperty)
        .def<bool (Parameter_ifs::*)(const std::string &, const Value &)>("setProperty", &Parameter_ifs::setProperty)
        .def("setPropertyAsTxt", &Parameter_ifs::setPropertyAsTxt)
        .def("isValid", &Parameter_ifs::isValid)
        .def("isLocked", &Parameter_ifs::isLocked);

    py::class_<DeviceViewWrapper_ifs, PyDeviceViewWrapper>(m, "DeviceViewWrapper")
        .def(py::init<>())
        .def("addSignal", &DeviceViewWrapper_ifs::addSignal)
        .def("setActive", &DeviceViewWrapper_ifs::setActive)
        .def("removeFromActive", &DeviceViewWrapper_ifs::removeFromActive)
        .def("addToSelected", &DeviceViewWrapper_ifs::addToSelected)
        .def("removeFromSelected", &DeviceViewWrapper_ifs::removeFromSelected)
        .def("getActiveDevice", &DeviceViewWrapper_ifs::getActiveDevice, py::return_value_policy::reference)
        .def("getActiveDeviceSource", &DeviceViewWrapper_ifs::getActiveDeviceSource)
        .def("getActiveModule", &DeviceViewWrapper_ifs::getActiveModule, py::return_value_policy::reference)
        .def("getActiveModulePath", &DeviceViewWrapper_ifs::getActiveModulePath)
        .def("getSelected", &DeviceViewWrapper_ifs::getSelected, py::return_value_policy::reference)
        .def("getSelectedPath", &DeviceViewWrapper_ifs::getSelectedPath)
        .def("addAction", &DeviceViewWrapper_ifs::addAction);

    py::class_<ParameterViewWrapper_ifs, PyParameterViewWrapper>(m, "ParameterViewWrapper")
        .def<bool (ParameterViewWrapper_ifs::*)(size_t)>("setActive", &ParameterViewWrapper_ifs::setActive)
        .def<bool (ParameterViewWrapper_ifs::*)(const std::string &)>("setActive", &ParameterViewWrapper_ifs::setActive)
        .def("removeFromActive", &ParameterViewWrapper_ifs::removeFromActive)
        .def<bool (ParameterViewWrapper_ifs::*)(size_t)>("addToSelected", &ParameterViewWrapper_ifs::addToSelected)
        .def<bool (ParameterViewWrapper_ifs::*)(const std::string &)>("addToSelected",
                                                                      &ParameterViewWrapper_ifs::addToSelected)
        .def("removeFromSelected", &ParameterViewWrapper_ifs::removeFromSelected)
        .def("getActive", &ParameterViewWrapper_ifs::getActive, py::return_value_policy::reference)
        .def("getSelected", &ParameterViewWrapper_ifs::getSelected, py::return_value_policy::reference);

    py::class_<ExrtAction_ifs, PyExrtAction, std::unique_ptr<ExrtAction_ifs, py::nodelete>>(m, "ExrtAction")
        .def(py::init<>())
        .def("run", &ExrtAction_ifs::run)
        .def("getDescription", &ExrtAction_ifs::getDescription)
        .def("getInfo", &ExrtAction_ifs::getInfo);

    py::class_<IO_ifs, PyIO, std::unique_ptr<IO_ifs, py::nodelete>>(m, "pyIO")
        .def(py::init<const std::string &, const std::string &, const std::string &>())
        .def("readDocument", &IO_ifs::readDocument)
        .def("saveDocument", &IO_ifs::saveDocument)
        .def_readonly("filename_pattern_", &IO_ifs::filename_pattern_)
        .def_readonly("file_type_", &IO_ifs::file_type_)
        .def_readonly("file_info_", &IO_ifs::file_info_);

    m.def("isNum", [](std::string a) { return isNum(createDataType(a)); });
    m.def("normalizeType", [](std::string a) { return toString(createDataType(a)); });
    m.def("printer_debug", printer_debug);

    m.def("getDataSchema", getDataSchema);
    m.def("getDeviceViewWrapper", getDeviceViewWrapper, py::return_value_policy::reference);
    m.def("getParameterViewWrapper", getDeviceViewWrapper, py::return_value_policy::reference);
    m.def("getExrtAction", getExrtAction, py::return_value_policy::reference);
    m.def("getPyIO", getPyIO, py::return_value_policy::reference);
}
