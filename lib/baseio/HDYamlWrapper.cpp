#include <regex>

#include "common/CustomTypes.h"
//
#include "HDYamlWrapper.h"

/*
 * this class use lazy evalution fo building itself.
 * Vector, map, or value are created once, on the first call.
 */
HierarchicalDataYamlWrapper::HierarchicalDataYamlWrapper(const YAML::Node &value)  // better to use move semantic
    : value_(value), vector_(nullptr) {}

HierarchicalDataYamlWrapper::~HierarchicalDataYamlWrapper() {
    if (vector_) {
        if (isMap()) {
            for (auto i : *map_) delete i.second;
            delete map_;
        } else {
            for (auto i : *vector_) delete i;
            delete vector_;
        }
    }
}

bool HierarchicalDataYamlWrapper::isArray() const { return value_.IsSequence(); }

bool HierarchicalDataYamlWrapper::isMap() const { return value_.IsMap(); }

bool HierarchicalDataYamlWrapper::isValue() const { return value_.IsScalar(); }

Value HierarchicalDataYamlWrapper::getValue() const {
    switch (value_.Type()) {
    case YAML::NodeType::Scalar: {
        std::string val = value_.Scalar();

        static const std::regex null_exp("[ ]*null");
        static const std::regex spec_exp("[ ]*(false|true|null)");
        static const std::regex num_exp("[ ]*(\\+|\\-)?([0-9]+|[0-9]+(.|,)[0-9]*)");
        static const std::regex int_exp("[ ]*(\\+|\\-)?([0-9]+)");

        if (std::regex_match(val, num_exp)) {
            if (std::regex_match(val, int_exp)) return Value(val, DataType::i64);
            return Value(val, DataType::f64);
        } else if (std::regex_match(val, spec_exp)) {
            if (std::regex_match(val, null_exp)) {
                return Value();
            } else {
                return Value(val, DataType::bool_v);
            }
        }
        return Value(val);
    }
    case YAML::NodeType::Null:
        return Value();
    default:
        break;
    }
    return Value();
}

std::vector<HierarchicalData_ifs *> HierarchicalDataYamlWrapper::getArray() const {
    if (!isArray()) return std::vector<HierarchicalData_ifs *>();

    if (!vector_) {
        auto arr = value_;

        auto vector_ptr = (std::vector<HierarchicalData_ifs *> **)&vector_;
        (*vector_ptr) = new std::vector<HierarchicalData_ifs *>();
        (*vector_ptr)->reserve(arr.size());

        for (const auto &i : arr) {
            (*vector_ptr)->push_back(new HierarchicalDataYamlWrapper(i));
        }
    }
    return *vector_;
}

std::map<std::string, HierarchicalData_ifs *> HierarchicalDataYamlWrapper::getMap() const {
    if (!isMap()) return std::map<std::string, HierarchicalData_ifs *>();

    if (!map_) {
        auto obj = value_;

        auto map_ptr = (std::map<std::string, HierarchicalData_ifs *> **)&map_;
        (*map_ptr) = new std::map<std::string, HierarchicalData_ifs *>();
        for (auto i : obj) {
            (**map_ptr)[i.first.as<std::string>()] = new HierarchicalDataYamlWrapper(i.second);
        }
    }
    return *map_;
}

HierarchicalData_ifs *HierarchicalDataYamlWrapper::getArrayUnit(size_t index) const {
    if (!isArray()) return nullptr;

    if (!vector_) {
        auto arr = value_;

        auto vector_ptr = (std::vector<HierarchicalData_ifs *> **)&vector_;
        (*vector_ptr) = new std::vector<HierarchicalData_ifs *>();
        (*vector_ptr)->reserve(arr.size());

        for (const auto &i : arr) {
            (*vector_ptr)->push_back(new HierarchicalDataYamlWrapper(i));
        }
    }

    if (index < vector_->size()) return (*vector_)[index];

    return nullptr;
}

HierarchicalData_ifs *HierarchicalDataYamlWrapper::getMapUnit(std::string field) const {
    if (!isMap()) return nullptr;

    if (!map_) {
        auto obj = value_;

        auto map_ptr = (std::map<std::string, HierarchicalData_ifs *> **)&map_;
        (*map_ptr) = new std::map<std::string, HierarchicalData_ifs *>();
        for (auto i : obj) {
            (**map_ptr)[i.first.as<std::string>()] = new HierarchicalDataYamlWrapper(i.second);
        }
    }

    auto item = map_->find(field);
    if (item != map_->end()) return item->second;

    return nullptr;
}