
#include "convtemplate/ParameterFieldTree.h"

#include <cstring>

#include "common/StringProcessingTools.h"

/*
 *
 */

class ParameterFieldTreeArray : public ParameterFieldTree_ifs {
   public:
    ParameterFieldTreeArray(const DataSchema_ifs *data_schema, size_t dim) : dim_(dim), data_schema_(data_schema) {
        auto len = data_schema->getDims()[dim];
        vector_.reserve(len);
        for (size_t i = 0; i < len; i++) vector_.push_back(newParameterFieldTree(data_schema, dim + 1));
    }

    [[nodiscard]] bool isArray() const override { return true; }

    [[nodiscard]] std::vector<HierarchicalData_ifs *> getArray() const override {
        auto s = vector_.size();
        std::vector<HierarchicalData_ifs *> ret(s);
        std::memcpy(ret.data(), vector_.data(), s * sizeof(HierarchicalData_ifs *));
        return ret;
    }

    [[nodiscard]] HierarchicalData_ifs *getArrayUnit(size_t id) const override {
        if (id >= vector_.size()) return nullptr;
        return (vector_[id]);
    }

    size_t getSize() override { return vector_.size(); }

    [[nodiscard]] bool addArrayUnit() override {
        auto len = data_schema_->getDims()[dim_];
        if (len != 0) return false;
        vector_.push_back(newParameterFieldTree(data_schema_, dim_ + 1));
        return true;
    }

    [[nodiscard]] bool removeArrayUnit(size_t index) override {
        auto len = data_schema_->getDims()[dim_];
        if ((len != 0) || (index >= vector_.size())) return false;
        delete vector_[index];
        vector_.erase(vector_.begin() + ptrdiff_t(index));
        return false;
    }

    // vec.erase(vec.begin()+1);

   private:
    const DataSchema_ifs *data_schema_;
    const size_t dim_ = 0;
    std::vector<ParameterFieldTree_ifs *> vector_;
};

class ParameterFieldTreeMap : public ParameterFieldTree_ifs {
   public:
    // typedef std::pair<std::string, HierarchicalData_ifs *> tuple_t;

    explicit ParameterFieldTreeMap(const DataSchema_ifs *data_schema) {
        auto map_list = data_schema->getMapList();
        vecmap_.reserve(map_list.size());
        for (auto i : map_list) vecmap_.push_back({i->name_, newParameterFieldTree(i)});

        for (auto &i : vecmap_) map_[i.first] = i.second;
    }

    [[nodiscard]] bool isMap() const override { return true; }

    [[nodiscard]] std::vector<std::pair<std::string, HierarchicalData_ifs *>> getMap() const override {
        // std::map<std::string, HierarchicalData_ifs *> ret;
        // for (auto &i : vecmap_) {
        //     ret[i.first] = i.second;
        // }
        // return ret
        return vecmap_;
    }

    [[nodiscard]] HierarchicalData_ifs *getMapUnit(const std::string &id) const override {
        auto map_uint = map_.find(id);
        if (map_uint != map_.end()) return (map_uint->second);
        return nullptr;
    }

    size_t getSize() override { return map_.size(); }

   private:
    getMapReturn_t vecmap_;
    std::map<std::string, HierarchicalData_ifs *> map_;
};

class ParameterFieldTreeValue : public ParameterFieldTree_ifs {
   public:
    explicit ParameterFieldTreeValue(const DataSchema_ifs *data_schema) {
        type_ = createDataType(data_schema->getType());
        // value_ = data_schema->default_value_;
        if (!value_) {
            if (isString(type_)) {
                value_ = Value(std::string(""), DataType::str_v);
            } else
                value_ = Value(std::string("0"), type_);
        }
    }

    [[nodiscard]] bool isValue() const override { return true; }

    [[nodiscard]] Value getValue() const override { return value_; }

    [[maybe_unused]] bool setValue(const Value &data) override {
        auto v = Value(data, type_);
        auto status = bool(v);
        if (status) value_ = v;
        return status;
    }

    [[maybe_unused]] bool setValue(const std::string &data) override {
        auto v = Value(data, type_);
        auto status = bool(v);
        if (status) value_ = v;
        return status;
    }

   private:
    DataType type_ = DataType::none_v;
    Value value_ = Value();
};

bool ParameterFieldTree_ifs::setValue(const std::string &prop_path, const Value &value, std::string &error_message) {
    auto ptr = (ParameterFieldTree_ifs *)getBranch(this, prop_path);
    if (ptr == nullptr || !ptr->isValue()) {
        error_message = "non-existent path";
        return false;
    }

    if (!ptr->setValue(value)) {
        error_message = "incompatible types";
        return false;
    }
    return true;
}

bool ParameterFieldTree_ifs::removeUnit(const std::string &prop_path, std::string &error_message) {
    auto path = lastCharPos(prop_path, '/');

    auto ptr = (ParameterFieldTree_ifs *)getBranch(this, path.first);
    if (!isNumber(path.second) || !ptr->isArray()) return false;

    return ptr->removeArrayUnit(size_t(std::stoi(path.second)));
}

bool ParameterFieldTree_ifs::setValue(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data,
                                      std::string &error_message) {
    auto path = lastCharPos(prop_path, '/');

    auto ptr = (ParameterFieldTree_ifs *)getBranch(this, path.first);
    if (!isNumber(path.second) || !ptr->isArray()) return false;

    auto pos = size_t(std::stoi(path.second));

    if ((pos != 0) && (ptr->getArrayUnit(pos - 1) == nullptr)) return false;

    if (ptr->getArrayUnit(pos) == nullptr) return ptr->addArrayUnit();

    return false;
}

bool ParameterFieldTree_ifs::setValueAsTxt(const std::string &prop_path, const std::string &value,
                                           std::string &error_message) {
    auto ptr = (ParameterFieldTree_ifs *)getBranch(this, prop_path);
    if (ptr == nullptr || !ptr->isValue()) {
        error_message = "non-existent path";
        return false;
    }

    if (!ptr->setValue(value)) {
        error_message = "incompatible types";
        return false;
    }
    return true;
}

ParameterFieldTree_ifs *newParameterFieldTree(const DataSchema_ifs *data_schema, size_t dim) {
    if (data_schema == nullptr) return nullptr;
    if (data_schema->isArray() && (dim < data_schema->getDims().size()))
        return new ParameterFieldTreeArray(data_schema, dim);
    if (data_schema->isMap()) return new ParameterFieldTreeMap(data_schema);
    return new ParameterFieldTreeValue(data_schema);
}