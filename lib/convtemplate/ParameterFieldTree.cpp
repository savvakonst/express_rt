
#include "convtemplate/ParameterFieldTree.h"

#include <cstring>

ParameterFieldTree::StructType getStructType(DataSchema_ifs *data_schema) {
    if (data_schema->isArray()) return ParameterFieldTree::StructType::array;
    if (data_schema->isMap()) return ParameterFieldTree::StructType::map;
    return ParameterFieldTree::StructType::value;
}
/*
 *
 */

class ParameterFieldTreeArray : public ParameterFieldTree_ifs {
   public:
    ParameterFieldTreeArray(DataSchema_ifs *data_schema, size_t dim) {
        auto len = data_schema->getDims()[dim];
        vector_.reserve(len);
        for (size_t i = 0; i < len; i++) vector_.push_back(newParameterFieldTree(data_schema, dim + 1));
    }

    [[nodiscard]] bool isArray() const override { return true; }

    [[nodiscard]] std::vector<HierarchicalData_ifs *> getArray() const override {
        auto s = vector_.size();
        std::vector<HierarchicalData_ifs *> ret(s);
        std::memcpy(ret.data(), vector_.data(), s);
        return ret;
    }

    [[nodiscard]] HierarchicalData_ifs *getArrayUnit(size_t id) const override {
        if (id >= vector_.size()) return nullptr;
        return (vector_[id]);
    }

    [[nodiscard]] virtual bool setArrayUnit(size_t index, ParameterFieldTree_ifs *data) { return false; }

   private:
    std::vector<ParameterFieldTree_ifs *> vector_;
};

class ParameterFieldTreeMap : public ParameterFieldTree_ifs {
   public:
    //typedef std::pair<std::string, HierarchicalData_ifs *> tuple_t;

    ParameterFieldTreeMap(DataSchema_ifs *data_schema) {
        auto map_list = data_schema->getMapList();
        vecmap_.reserve(map_list.size());
        for (auto i : map_list) vecmap_.push_back({i->name_, newParameterFieldTree(i)});

        for (auto &i : vecmap_) map_[i.first] = i.second;
    }

    [[nodiscard]] bool isMap() const override { return true; }

    [[nodiscard]] std::vector<std::pair<std::string, HierarchicalData_ifs *>> getMap() const override {
        //std::map<std::string, HierarchicalData_ifs *> ret;
        //for (auto &i : vecmap_) {
        //    ret[i.first] = i.second;
        //}
        //return ret
        return vecmap_;
    }

    [[nodiscard]] HierarchicalData_ifs *getMapUnit(std::string id) const override {
        auto map_uint = map_.find(id);
        if (map_uint != map_.end()) return (map_uint->second);
        return nullptr;
    }

   private:
    getMapReturn_t vecmap_;
    std::map<std::string, HierarchicalData_ifs *> map_;
};
#include "iostream"
class ParameterFieldTreeValue : public ParameterFieldTree_ifs {
   public:
    ParameterFieldTreeValue(DataSchema_ifs *data_schema) {
        type_ = createDataType(data_schema->getType());
        //value_ = data_schema->default_value_;
        if (!value_) {
            if(isString(type_)) {
                value_ = Value(std::string(""), DataType::str_v);
            }
            else
                value_ = Value(std::string("0"), type_);
        }
    }

    [[nodiscard]] bool isValue() const override { return true; }

    [[nodiscard]] Value getValue() const override { return value_; }

    [[maybe_unused]] virtual bool setValue(const Value &data) override {

        value_ = data;
        return true;
    }

    [[maybe_unused]] virtual bool setValue(const std::string &data) override {
        value_ = Value(data, type_);
        return true;
    }

   private:
    DataType type_ = DataType::none_v;
    Value value_ = Value();
};

ParameterFieldTree_ifs *newParameterFieldTree(DataSchema_ifs *data_schema, size_t dim) {
    if (data_schema->isArray()) return new ParameterFieldTreeArray(data_schema, dim);
    if (data_schema->isMap()) return new ParameterFieldTreeMap(data_schema);
    return new ParameterFieldTreeValue(data_schema);
}

/*
 *
 *
 */

ParameterFieldTree::ParameterFieldTree(DataSchema_ifs *data_schema, size_t dim)
    : struct_type_(getStructType(data_schema)) {
    switch (struct_type_) {
    case array: {
        auto len = data_schema->getDims()[dim];
        vector_.reserve(len);
        for (size_t i = 0; i < len; i++) vector_.push_back(ParameterFieldTree(data_schema, dim + 1));
        break;
    }
    case map: {
        auto map_list = data_schema->getMapList();
        vecmap_.reserve(map_list.size());
        for (auto i : map_list) vecmap_.push_back({i->name_, ParameterFieldTree(i)});

        for (auto &i : vecmap_) map_[i.first] = &i.second;

        break;
    }
    case value: {
        type_ = createDataType(data_schema->getType());
        value_ = data_schema->default_value_;
        break;
    }
    }
}

ParameterFieldTree::~ParameterFieldTree() {}

std::vector<HierarchicalData_ifs *> ParameterFieldTree::getArray() const {
    std::vector<HierarchicalData_ifs *> ret;

    ret.reserve(vector_.size());
    for (auto &i : vector_) {
        ret.push_back((ParameterFieldTree *)&i);
    }
    return ret;
}

ParameterFieldTree::getMapReturn_t ParameterFieldTree::getMap() const {
    /*
    std::map<std::string, HierarchicalData_ifs *> ret;
    for (auto &i : vecmap_) {
        ret[i.first] = (HierarchicalData_ifs *)&i.second;
    }
    return ret;
    */
    getMapReturn_t ret;
    ret.reserve(ret.size());
    for (auto &i : vecmap_) {
        ret.push_back({i.first,(HierarchicalData_ifs *)&i.second});
    }
    return ret;
}

HierarchicalData_ifs *ParameterFieldTree::getArrayUnit(size_t id) const {
    if (id >= vector_.size()) return nullptr;
    return (ParameterFieldTree *)&(vector_[id]);
}

HierarchicalData_ifs *ParameterFieldTree::getMapUnit(std::string id) const {
    auto map_uint = map_.find(id);
    if (map_uint != map_.end()) return (ParameterFieldTree *)&(map_uint->second);
    return nullptr;
}

bool ParameterFieldTree::setArrayUnit(size_t index, HierarchicalData_ifs *data) { return false; }

//bool ParameterFieldTree::setMapUnit(size_t field_name, HierarchicalData_ifs *data) { return false; }

bool ParameterFieldTree::setValue(Value data) { return false; }

bool ParameterFieldTree::setValue(std::string data) { return false; }
