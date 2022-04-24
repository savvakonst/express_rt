//
// Created by SVK on 22.12.2021.
//

#ifndef EXAMPLE_BINDER_H
#define EXAMPLE_BINDER_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "GUI/WidgetWrappers.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"
#include "common/ExrtAction_ifs.h"
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/Parameter_ifs.h"
#include "convtemplate/PrmBuffer_ifs.h"
#include "device/Device_ifs.h"
#include "device/ModuleStream_ifs.h"

PYBIND11_MAKE_OPAQUE(std::map<std::string, PrmBuffer_ifs *>);
/*
 *
 *
 */
namespace py = pybind11;

class PyDataSchema : public DataSchema_ifs {
   public:
    PyDataSchema(const std::string &name, const std::string &description)  //
        : DataSchema_ifs(name, description) {}

    ~PyDataSchema() override {
        std::cout << "~PyDataSchema()\n";
        if (handle_) handle_.dec_ref();
    }

    void init(ExtensionManager *manager) override { PYBIND11_OVERRIDE_PURE(void, DataSchema_ifs, init, manager); }

    void setOwnerShip(py::handle handle) { handle_ = handle; }

    [[nodiscard]] std::string getType() const override { PYBIND11_OVERRIDE_PURE(std::string, DataSchema_ifs, getType); }

    [[nodiscard]] std::string getRepresentationType() const override {
        PYBIND11_OVERRIDE_PURE(std::string, DataSchema_ifs, getRepresentationType);
    }

    [[nodiscard]] bool isValid() const override { PYBIND11_OVERRIDE_PURE(bool, DataSchema_ifs, isValid); }

    [[nodiscard]] bool isMap() const override { PYBIND11_OVERRIDE_PURE(bool, DataSchema_ifs, isMap); }

    [[nodiscard]] bool isArray() const override { PYBIND11_OVERRIDE_PURE(bool, DataSchema_ifs, isArray); }

    [[nodiscard]] std::vector<size_t> getDims() const override {
        PYBIND11_OVERRIDE_PURE(std::vector<size_t>, DataSchema_ifs, getDims);
    }

    [[nodiscard]] std::vector<DataSchema_ifs *> getMapList() const override {
        PYBIND11_OVERRIDE_PURE(std::vector<DataSchema_ifs *>, DataSchema_ifs, getMapList);
    }

    bool validate(HierarchicalData_ifs *data) const override {
        PYBIND11_OVERRIDE_PURE(bool, DataSchema_ifs, validate, data);
    }

    py::handle handle_;
};

class PyHierarchicalData : public HierarchicalData_ifs {
   public:
    ~PyHierarchicalData() override = default;
    ;

    [[nodiscard]] bool isArray() const override {
        PYBIND11_OVERRIDE_PURE(bool,                 /* Return type */
                               HierarchicalData_ifs, /* Parent class */
                               isArray               /* Name of function in C++ (must match Python name) */
        );
    }

    [[nodiscard]] bool isMap() const override { PYBIND11_OVERRIDE_PURE(bool, HierarchicalData_ifs, isMap); }

    [[nodiscard]] bool isValue() const override { PYBIND11_OVERRIDE_PURE(bool, HierarchicalData_ifs, isValue); }

    [[nodiscard]] Value getValue() const override { PYBIND11_OVERRIDE_PURE(Value, HierarchicalData_ifs, getValue); }

    [[nodiscard]] std::vector<HierarchicalData_ifs *> getArray() const override {
        PYBIND11_OVERRIDE_PURE(std::vector<HierarchicalData_ifs *>, HierarchicalData_ifs, getArray);
    }

    [[nodiscard]] getMapReturn_t getMap() const override {
        PYBIND11_OVERRIDE_PURE(getMapReturn_t, HierarchicalData_ifs, getMap);
    }

    [[nodiscard]] HierarchicalData_ifs *getArrayUnit(size_t arg) const override {
        PYBIND11_OVERRIDE_PURE(HierarchicalData_ifs *, HierarchicalData_ifs, getArrayUnit, arg);
    }

    [[nodiscard]] HierarchicalData_ifs *getMapUnit(const std::string &arg) const override {
        PYBIND11_OVERRIDE_PURE(HierarchicalData_ifs *, HierarchicalData_ifs, getMapUnit, arg);
    }
};

class PyModule : public Module_ifs {
   public:
    ~PyModule() override = default;

    [[nodiscard]] bool hasTransceiver() const override { PYBIND11_OVERRIDE_PURE(bool, Module_ifs, hasTransceiver); }

    [[nodiscard]] EthernetSettings getSrcAddress() const override {
        PYBIND11_OVERRIDE_PURE(EthernetSettings, Module_ifs, getSrcAddress);
    }

    [[nodiscard]] bool isAvailable() const override { PYBIND11_OVERRIDE_PURE(bool, Module_ifs, isAvailable); }

    [[nodiscard]] std::string getID() const override { PYBIND11_OVERRIDE_PURE(std::string, Module_ifs, getID); }

    [[nodiscard]] std::string getModulePath(bool full_path = true) const override {
        PYBIND11_OVERRIDE_PURE(std::string, Module_ifs, getModulePath, full_path);
    }

    [[nodiscard]] Module_ifs *getParentModule() const override {
        PYBIND11_OVERRIDE_PURE(Module_ifs *, Module_ifs, getParentModule);
    }

    bool setParentModule(Module_ifs *parent) override {
        PYBIND11_OVERRIDE_PURE(Module_ifs *, Module_ifs, setParentModule, parent);
    }

    [[nodiscard]] std::list<Module_ifs *> getSubModulesFromPath(const std::string &module_path) const override {
        PYBIND11_OVERRIDE_PURE(std::list<Module_ifs *>, Module_ifs, getSubModulesFromPath, module_path);
    }

    typedef std::map<std::string, PrmBuffer_ifs *> prm_buff_map_t;

    [[nodiscard]] std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() override {
        PYBIND11_OVERRIDE_PURE(prm_buff_map_t, Module_ifs, getPrmBufferMap);
    }

    [[nodiscard]] const DataSchema_ifs *getPropertySchema() const override {
        PYBIND11_OVERRIDE_PURE(const DataSchema_ifs *, Module_ifs, getPropertySchema);
    }

    [[nodiscard]] std::string printProperties(const std::string &indent) const override {
        PYBIND11_OVERRIDE_PURE(std::string, Module_ifs, printProperties, indent);
    }

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override {
        PYBIND11_OVERRIDE_PURE(const HierarchicalData_ifs *, Module_ifs, getProperty, prop_path);
    }

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override {
        PYBIND11_OVERRIDE_PURE(std::string, Module_ifs, getPropertyAsTxt, prop_path);
    }

    bool setProperty(const std::string &prop_path, const Value &value) override {
        PYBIND11_OVERRIDE_PURE(bool, Module_ifs, setProperty, prop_path, value);
    }

    bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) override {
        PYBIND11_OVERRIDE_PURE(bool, Module_ifs, setProperty, prop_path, hierarchical_data);
    }

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override {
        PYBIND11_OVERRIDE_PURE(bool, Module_ifs, setPropertyAsTxt, prop_path, value);
    }

    bool removeProperty(const std::string &prop_path) override {
        PYBIND11_OVERRIDE_PURE(bool, Module_ifs, removeProperty, prop_path);
    }

    [[nodiscard]] bool isChannelAvailable(const std::string &prop_path) const override {
        PYBIND11_OVERRIDE_PURE(bool, Module_ifs, isChannelAvailable, prop_path);
    }
    [[nodiscard]] Value getChannelProperty(const std::string &channel, const std::string &type) const override {
        PYBIND11_OVERRIDE_PURE(Value, Module_ifs, getChannelProperty, channel, type);
    }

    [[nodiscard]] bool storeTaskToBuffer(void *pointer) const override {
        PYBIND11_OVERRIDE_PURE(bool, Module_ifs, storeTaskToBuffer, pointer);
    }

    [[nodiscard]] size_t getTaskSize() const override { PYBIND11_OVERRIDE_PURE(size_t, Module_ifs, getTaskSize); }

    typedef std::vector<std::pair<std::string, Module_ifs *>> sub_modules_map_t;
    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const override {
        PYBIND11_OVERRIDE_PURE(sub_modules_map_t, Module_ifs, getSubModules);
    }

    ModuleStream_ifs *createModuleStream() override {
        PYBIND11_OVERRIDE_PURE(ModuleStream_ifs *, Module_ifs, createModuleStream);
    }

    ModuleStream_ifs *getModuleStream() override {
        PYBIND11_OVERRIDE_PURE(ModuleStream_ifs *, Module_ifs, getModuleStream);
    }

    bool deattachModuleStream() override { PYBIND11_OVERRIDE_PURE(bool, Module_ifs, deattachModuleStream); }
};

class PyParameter : public Parameter_ifs {
   public:
    ~PyParameter() override = default;

    [[nodiscard]] PrmBuffer_ifs *createBuffer() const override {
        PYBIND11_OVERRIDE_PURE(PrmBuffer_ifs *, Parameter_ifs, createBuffer);
    }

    [[nodiscard]] std::string getType() const override { PYBIND11_OVERRIDE_PURE(std::string, Parameter_ifs, getType); }

    const DataSchema_ifs *getPropertySchema() const override {
        PYBIND11_OVERRIDE_PURE(const DataSchema_ifs *, Parameter_ifs, getPropertySchema);
    }

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override {
        PYBIND11_OVERRIDE_PURE(const HierarchicalData_ifs *, Parameter_ifs, getProperty, prop_path);
    }

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override {
        PYBIND11_OVERRIDE_PURE(std::string, Parameter_ifs, getPropertyAsTxt, prop_path);
    }

    bool setProperty(const std::string &prop_path, const Value &value) override {
        PYBIND11_OVERRIDE_PURE(bool, Parameter_ifs, setProperty, prop_path, value);
    }

    bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) override {
        PYBIND11_OVERRIDE_PURE(bool, Parameter_ifs, setProperty, prop_path, hierarchical_data);
    }

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override {
        PYBIND11_OVERRIDE_PURE(bool, Parameter_ifs, setPropertyAsTxt, prop_path, value);
    }

    [[nodiscard]] bool isValid() const override { PYBIND11_OVERRIDE_PURE(bool, Parameter_ifs, isValid); }

   protected:
    friend PrmBuffer_ifs;
    PrmBuffer_ifs *prm_buffer_ = nullptr;
};

class PyDeviceViewWrapper : public DeviceViewWrapper_ifs {
   public:
    QWidget *getWidget() override { return nullptr; }

    status addSignal(Signal_ifs *signal) override {
        PYBIND11_OVERRIDE_PURE(status, DeviceViewWrapper_ifs, addSignal, signal);
    }

    bool setActive(const std::string &source, const std::string &path) override {
        PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, setActive, source, path);
    }

    bool removeFromActive() override { PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, addToSelected); }

    bool addToSelected(const std::string &source, const std::string &path) override {
        PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, addToSelected, source, path);
    }

    bool removeFromSelected(const std::string &source, const std::string &path) override {
        PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, removeFromSelected, source, path);
    }

    Device_ifs *getActiveDevice() override {
        PYBIND11_OVERRIDE_PURE(Device_ifs *, DeviceViewWrapper_ifs, getActiveDevice);
    }

    std::string getActiveDeviceSource() override {
        PYBIND11_OVERRIDE_PURE(std::string, DeviceViewWrapper_ifs, getActiveDeviceSource);
    }

    Module_ifs *getActiveModule() override {
        PYBIND11_OVERRIDE_PURE(Module_ifs *, DeviceViewWrapper_ifs, getActiveModule);
    }
    typedef std::pair<std::string, std::string> strPair_t;

    std::pair<std::string, std::string> getActiveModulePath() override {
        PYBIND11_OVERRIDE_PURE(strPair_t, DeviceViewWrapper_ifs, getActiveModulePath);
    }

    std::vector<Module_ifs *> getSelected() override {
        PYBIND11_OVERRIDE_PURE(std::vector<Module_ifs *>, DeviceViewWrapper_ifs, getSelected);
    }

    std::vector<std::pair<std::string, std::string>> getSelectedPath() override {
        PYBIND11_OVERRIDE_PURE(std::vector<strPair_t>, DeviceViewWrapper_ifs, getActiveDevicePath);
    }

    void addAction(ExrtAction_ifs *action) override {
        PYBIND11_OVERRIDE_PURE(void, DeviceViewWrapper_ifs, addAction, action);
    }
};

class PyParameterViewWrapper : public ParameterViewWrapper_ifs {
   public:
    QWidget *getWidget() override { return nullptr; }

    status addSignal(Signal_ifs *signal) override {
        PYBIND11_OVERRIDE_PURE(status, ParameterViewWrapper_ifs, addSignal, signal);
    }

    ConversionTemplate *currentConversionTemplate() override {
        PYBIND11_OVERRIDE_PURE(ConversionTemplate *, ParameterViewWrapper_ifs, currentConversionTemplate);
    }

    bool setActive(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_ifs, setActive, row_index);
    }

    bool setActive(const std::string &name) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_ifs, setActive, name);
    }

    bool removeFromActive() override { PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_ifs, removeFromActive); }

    bool addToSelected(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_ifs, addToSelected, row_index);
    }

    bool addToSelected(const std::string &name) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_ifs, addToSelected, name);
    }

    bool removeFromSelected(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_ifs, removeFromSelected, row_index);
    }

    Parameter_ifs *getActive() override {
        PYBIND11_OVERRIDE_PURE(Parameter_ifs *, ParameterViewWrapper_ifs, getActive);
    }

    std::vector<Parameter_ifs *> getSelected() override {
        PYBIND11_OVERRIDE_PURE(std::vector<Parameter_ifs *>, ParameterViewWrapper_ifs, getSelected);
    }
};

class PyConversionTemplateViewWrapper : public ConversionTemplateViewWrapper_if {
   public:
    QWidget *getWidget() override { return nullptr; }

    status addSignal(Signal_ifs *signal) override {
        PYBIND11_OVERRIDE_PURE(status, ConversionTemplateViewWrapper_if, addSignal, signal);
    }

    bool setActive(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ConversionTemplateViewWrapper_if, setActive, row_index);
    }

    bool setActive(const std::string &source) override {
        PYBIND11_OVERRIDE_PURE(bool, ConversionTemplateViewWrapper_if, setActive, source);
    }

    bool removeFromActive() override {
        PYBIND11_OVERRIDE_PURE(bool, ConversionTemplateViewWrapper_if, removeFromActive);
    }

    bool addToSelected(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ConversionTemplateViewWrapper_if, addToSelected, row_index);
    }

    bool addToSelected(const std::string &name) override {
        PYBIND11_OVERRIDE_PURE(bool, ConversionTemplateViewWrapper_if, addToSelected, name);
    }

    bool removeFromSelected(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ConversionTemplateViewWrapper_if, removeFromSelected, row_index);
    }

    ConversionTemplate *getActive() override {
        PYBIND11_OVERRIDE_PURE(ConversionTemplate *, ConversionTemplateViewWrapper_if, getActive);
    }

    std::vector<ConversionTemplate *> getSelected() override {
        PYBIND11_OVERRIDE_PURE(std::vector<ConversionTemplate *>, ConversionTemplateViewWrapper_if, getSelected);
    }
};

class PyExrtAction : public ExrtAction_ifs {
   public:
    ~PyExrtAction() override {
        std::cout << "~PyExrtAction()\n";
        if (handle_) handle_.dec_ref();
    }

    void setOwnerShip(py::handle handle) { handle_ = handle; }

    bool run() override { PYBIND11_OVERRIDE_PURE(bool, ExrtAction_ifs, run); }

    std::string getDescription() override { PYBIND11_OVERRIDE_PURE(std::string, ExrtAction_ifs, getDescription); }

    std::string getInfo() override { PYBIND11_OVERRIDE_PURE(std::string, ExrtAction_ifs, getInfo); }

    py::handle handle_;
};

#include "common/IO_ifs.h"

class ExtensionManager;

class PyIO : public IO_ifs {
   public:
    PyIO(const std::string &filename_pattern, const std::string &file_type, const std::string &file_info)
        : IO_ifs(filename_pattern, file_type, file_info) {}

    ~PyIO() override = default;

    bool readDocument(ExtensionManager *manager, const std::string &source_path) override {
        PYBIND11_OVERRIDE_PURE(bool, IO_ifs, readDocument, manager, source_path);
    }

    bool saveDocument(ExtensionManager *manager, const std::string &id, const std::string &dst_path) override {
        PYBIND11_OVERRIDE_PURE(bool, IO_ifs, saveDocument, manager, id, dst_path);
    }

    bool saveDocument(ExtensionManager *manager, void *obj_ptr, const std::string &dst_path) override {
        PYBIND11_OVERRIDE_PURE(bool, IO_ifs, saveDocument, manager, obj_ptr, dst_path);
    }

};

#endif  // EXAMPLE_BINDER_H
