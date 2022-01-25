#ifndef EXRT_DATASCHEMA_H
#define EXRT_DATASCHEMA_H

#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"

class DataSchema : public DataSchema_ifs {
   public:
    DataSchema(DataType type, const std::string &name, const std::string &description,
               const std::vector<size_t> &dims = {})  //
        : type_(type), dims_(dims), DataSchema_ifs(name, description) {}

    DataSchema(const std::vector<DataSchema_ifs *> &map, const std::string &name, const std::string &description,
               const std::vector<size_t> &dims = {})  //
        : map_(map), DataSchema_ifs(name, description) {}

    DataSchema(const EnumInfo_t &enums, const std::string &name, const std::string &description,
               const std::vector<size_t> &dims = {})  //
        : DataSchema_ifs(name, description) {
        enums_ = enums;
        if (enums_.size()) type_ = enums_.front().second.type_;
    }

    std::string getType() const override { return toString(type_); }

    bool isValid() const {
        bool is_none = type_ == DataType::none_v;
        bool is_map = bool(map_.size());
        return is_none == is_map;
    }

    virtual std::string getRepresentationType() const {
        // TODO :
        if (enums_.size()) return "enum";
        else if (isNum(type_)) {
            return "number";
        } else if (isBool(type_)) {
            return "bool";
        } else if (type_ == lstr_v) {
            return "large_text";
        } else if (type_ == str_v) {
            return "text";
        }
        return "text";
    }

    virtual bool isMap() const override { return bool(map_.size()); }

    virtual bool isArray() const override { return bool(dims_.size()); }

    virtual const std::vector<size_t> getDims() const override { return dims_; }

    virtual const std::vector<DataSchema_ifs *> getMapList() const override { return map_; }

    virtual bool validate(HierarchicalData_ifs *data) const override { return true; }

   private:
    DataType type_ = DataType::none_v;
    std::vector<DataSchema_ifs *> map_;
    std::vector<size_t> dims_;
};

#endif