//
// Created by SVK on 22.12.2021.
//

#ifndef EXAMPLE_BINDER_H
#define EXAMPLE_BINDER_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "common/CustomTypes.h"
#include "common/FieldInfo.h"
/* helper class definition
 *
 *
 */
namespace py = pybind11;


class PyDataSchema : public DataSchema_ifs {
public:

    PyDataSchema(const std::string &name, const std::string &description) //
            : DataSchema_ifs(name, description) {
    }

    ~PyDataSchema() override {
        std::cout << "~PyDataSchema()\n";
        if (handle_)
            handle_.dec_ref();
    }

    void setOwnerShip(py::handle handle) {
        handle_ = handle;
    }

    virtual std::string getType() const override {
        PYBIND11_OVERRIDE_PURE(
                std::string,
                DataSchema_ifs,
                getType
        );
    }


    std::string getRepresentationType() const override {
        PYBIND11_OVERRIDE_PURE(
                std::string,
                DataSchema_ifs,
                getRepresentationType
        );
    }

    bool isValid() const override {
        PYBIND11_OVERRIDE_PURE(
                bool,
                DataSchema_ifs,
                isValid
        );
    }

    bool isMap() const override {
        PYBIND11_OVERRIDE_PURE(
                bool,
                DataSchema_ifs,
                isMap
        );
    }

    bool isArray() const override {
        PYBIND11_OVERRIDE_PURE(
                bool,
                DataSchema_ifs,
                isArray
        );
    }


    const std::vector<size_t> getDims() const override {
        PYBIND11_OVERRIDE_PURE(
                std::vector<size_t>,
                DataSchema_ifs,
                getDims
        );
    }

    const std::vector<DataSchema_ifs *> getMapList() const override {
        PYBIND11_OVERRIDE_PURE(
                std::vector<DataSchema_ifs *>,
                DataSchema_ifs,
                getMapList
        );
    }

    virtual bool validate(HierarchicalData_ifs *data) const override {
        PYBIND11_OVERRIDE_PURE(
                bool,
                DataSchema_ifs,
                validate,
                data
        );
    }

    py::handle handle_;
};

class PyHierarchicalData : public HierarchicalData_ifs {
public:
    ~PyHierarchicalData() override {

    };


    bool isArray() const override {
        PYBIND11_OVERRIDE_PURE(
                bool, /* Return type */
                HierarchicalData_ifs,      /* Parent class */
                isArray          /* Name of function in C++ (must match Python name) */
        );
    }

    [[nodiscard]] bool isMap() const override {
        PYBIND11_OVERRIDE_PURE(
                bool,
                HierarchicalData_ifs,
                isMap
        );
    }

    [[nodiscard]] bool isValue() const override {
        PYBIND11_OVERRIDE_PURE(
                bool,
                HierarchicalData_ifs,
                isValue
        );
    }

    [[nodiscard]] Value getValue() const override {
        PYBIND11_OVERRIDE_PURE(
                Value,
                HierarchicalData_ifs,
                getValue
        );
    }

    [[nodiscard]] std::vector<HierarchicalData_ifs *>
    getArray() const override {
        PYBIND11_OVERRIDE_PURE(
                std::vector<HierarchicalData_ifs *>,
                HierarchicalData_ifs,
                getArray
        );
    }

    typedef std::map<std::string, HierarchicalData_ifs *> getMapRet_t;

    [[nodiscard]] std::map<std::string, HierarchicalData_ifs *> getMap() const override {
        PYBIND11_OVERRIDE_PURE(
                getMapRet_t,
                HierarchicalData_ifs,
                getMap
        );
    }

    [[nodiscard]] HierarchicalData_ifs *getArrayUint(size_t arg) const
    override {
        PYBIND11_OVERRIDE_PURE(
                HierarchicalData_ifs *,
                HierarchicalData_ifs,
                getArrayUint,
                arg
        );
    }

    [[nodiscard]] HierarchicalData_ifs *getMapUint(std::string arg) const override {
        PYBIND11_OVERRIDE_PURE(
                HierarchicalData_ifs *,
                HierarchicalData_ifs,
                getMapUint,
                arg
        );
    }
};


inline std::string indentationPrinter_debug(DataSchema_ifs *arg, const std::string &intend) {

    std::string field_name = intend + arg->name_;
    auto dims = arg->getDims();
    for (auto &i: dims)
        field_name += "[" + std::to_string(i) + "]";


    std::string type = ":\n";
    auto map_list = arg->getMapList();

    if (arg->isMap())
        for (auto i: map_list)
            type += indentationPrinter_debug(i, intend + "  ");

    else {
        type = ": " + arg->getType() + "\n";
    }
    field_name += type;
    return field_name;
}


#endif //EXAMPLE_BINDER_H
