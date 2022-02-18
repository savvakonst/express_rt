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
#include "common/ExtensionManager.h"
/* helper class definition
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

    const std::vector<size_t> getDims() const override {
        PYBIND11_OVERRIDE_PURE(std::vector<size_t>, DataSchema_ifs, getDims);
    }

    [[nodiscard]] const std::vector<DataSchema_ifs *> getMapList() const override {
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

    [[nodiscard]] HierarchicalData_ifs *getMapUnit(std::string arg) const override {
        PYBIND11_OVERRIDE_PURE(HierarchicalData_ifs *, HierarchicalData_ifs, getMapUnit, arg);
    }
};

#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/Parameter_ifs.h"
#include "device/Device.h"

class PyDeviceViewWrapper : public DeviceViewWrapper_ifs {
   public:
    QWidget *getWidget() override { return nullptr; }

    status addSignal(Signal_ifs *signal) override {
        PYBIND11_OVERRIDE_PURE(status, DeviceViewWrapper_ifs, addSignal, signal);
    }

    bool setActive(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, setActive, row_index);
    }

    bool setActive(const std::string &source, const std::string &path) override {
        PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, setActive, source, path);
    }

    bool removeFromActive() override { PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, addToSelected); }

    bool addToSelected(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, addToSelected, row_index);
    }

    bool removeFromSelected(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, DeviceViewWrapper_ifs, removeFromSelected, row_index);
    }

    Device *getActiveDevice() override { PYBIND11_OVERRIDE_PURE(Device *, DeviceViewWrapper_ifs, getActiveDevice); }

    Module_ifs *getActiveModule() override {
        PYBIND11_OVERRIDE_PURE(Module_ifs *, DeviceViewWrapper_ifs, getActiveModule);
    }

    std::vector<Module_ifs *> getSelected() override {
        PYBIND11_OVERRIDE_PURE(std::vector<Module_ifs *>, DeviceViewWrapper_ifs, getSelected);
    }
};

class PyParameterViewWrapper : public ParameterViewWrapper_if {
   public:
    QWidget *getWidget() override { return nullptr; }

    status addSignal(Signal_ifs *signal) override {
        PYBIND11_OVERRIDE_PURE(status, ParameterViewWrapper_if, addSignal, signal);
    }

    bool setActive(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_if, setActive, row_index);
    }

    bool setActive(const std::string &name) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_if, setActive, name);
    }

    bool removeFromActive() override { PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_if, removeFromActive); }

    bool addToSelected(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_if, addToSelected, row_index);
    }

    bool addToSelected(const std::string &name) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_if, addToSelected, name);
    }

    bool removeFromSelected(size_t row_index) override {
        PYBIND11_OVERRIDE_PURE(bool, ParameterViewWrapper_if, removeFromSelected, row_index);
    }

    Parameter_ifs *getActive() override { PYBIND11_OVERRIDE_PURE(Parameter_ifs *, ParameterViewWrapper_if, getActive); }

    std::vector<Parameter_ifs *> getSelected() override {
        PYBIND11_OVERRIDE_PURE(std::vector<Parameter_ifs *>, ParameterViewWrapper_if, getSelected);
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

#include "common/ExrtAction_ifs.h"

class PyExrtAction : public ExrtAction_ifs {
   public:
    bool run() override { PYBIND11_OVERRIDE_PURE(bool, ExrtAction_ifs, run); }

    std::string getDescription() override { PYBIND11_OVERRIDE_PURE(std::string, ExrtAction_ifs, getDescription); }

    std::string getInfo() override { PYBIND11_OVERRIDE_PURE(std::string, ExrtAction_ifs, getInfo); }
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
};

#endif  // EXAMPLE_BINDER_H
