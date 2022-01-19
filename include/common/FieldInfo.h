#ifndef EXRT_FIELDTYPE_H
#define EXRT_FIELDTYPE_H

#include <string>
#include <vector>

#include "CustomTypes.h"

/*
 * this is the data structure descriptor class
 */
typedef std::vector<std::pair<std::string, Value>> EnumInfo_t;

class ExtensionManager;

class DataSchema_ifs {
   public:
    DataSchema_ifs(const std::string &name, const std::string &description)  //
        : name_(name), description_(description) {}

    virtual ~DataSchema_ifs() = default;

    virtual void init(ExtensionManager* manager) = 0;

    [[nodiscard]] virtual std::string getType() const = 0;

    [[nodiscard]] virtual std::string getRepresentationType() const = 0;

    [[nodiscard]] virtual bool isValid() const = 0;

    [[nodiscard]] virtual bool isMap() const = 0;

    [[nodiscard]] virtual bool isArray() const = 0;

    [[nodiscard]] virtual const std::vector<size_t> getDims() const = 0;

    [[nodiscard]] virtual const std::vector<DataSchema_ifs *> getMapList() const = 0;

    virtual bool validate(HierarchicalData_ifs *data) const = 0;



    std::string name_;
    std::string description_;
    std::string help_;

    Value default_value_ = Value();
    EnumInfo_t enums_ = {};

   private:
};

inline std::string toString(DataSchema_ifs *arg, const std::string &intend) {
    std::string field_name = intend + arg->name_;
    auto dims = arg->getDims();
    for (auto &i : dims) field_name += "[" + std::to_string(i) + "]";

    std::string type = ":\n";
    auto map_list = arg->getMapList();

    if (arg->isMap())
        for (auto i : map_list) type += toString(i, intend + "  ");
    else {
        type = ": " + arg->getType() + "\n";
    }
    field_name += type;
    return field_name;
}


typedef size_t StatusFlags;

struct ResValue : Value {
    ResValue() : Value() {}

    ResValue(const Value &arg) : Value(arg) {}

    DataSchema_ifs *field_info_ = nullptr;
    StatusFlags data_status_ = 0;
};



#endif