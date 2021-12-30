
#include "KsdModule.h"

#include <regex>

/*
 *
 *
 *
 *
 */
TaskMapper::TaskMapper(DataType type) : struct_type_(StructType::value), type_(type) { size_ = getTSize(type_); }

TaskMapper::TaskMapper(size_t len, const TaskMapper& value) : struct_type_(StructType::array) {
    vector_.reserve(len);
    for (size_t i = 0; i < len; i++) {
        vector_.push_back(value);
    }
    size_ = len * value.size_;
}

TaskMapper::TaskMapper(std::vector<tuple> vecmap) : vecmap_(std::move(vecmap)), struct_type_(StructType::map) {
    size_ = 0;
    for (auto& i : vecmap_) {
        size_ += i.second.size_;
        map_[i.first] = &i.second;
    }
}

TaskMapper::~TaskMapper() {}

std::vector<HierarchicalData_ifs*> TaskMapper::getArray() const {
    std::vector<HierarchicalData_ifs*> ret;

    ret.reserve(vector_.size());
    for (auto& i : vector_) {
        ret.push_back((TaskMapper*)&i);
    }
    return ret;
}

std::map<std::string, HierarchicalData_ifs*> TaskMapper::getMap() const {
    // std::map<std::string, HierarchicalData_ifs*> ret;
    // for (auto& i : map_) {
    //     ret[i.first] = i.second;
    // }
    std::map<std::string, HierarchicalData_ifs*> ret;
    for (auto& i : vecmap_) {
        ret[i.first] = (HierarchicalData_ifs*)&i.second;
    }
    return ret;
}

HierarchicalData_ifs* TaskMapper::getArrayUint(size_t id) const {
    if (id >= vector_.size()) return nullptr;
    return (TaskMapper*)&(vector_[id]);
}

HierarchicalData_ifs* TaskMapper::getMapUint(std::string id) const {
    auto map_uint = map_.find(id);
    if (map_uint != map_.end()) return (TaskMapper*)&(map_uint->second);
    return nullptr;
}

bool TaskMapper::setValue(Value data) {
    if (isValue() && (data.type_ == type_)) {
        memcpy(ptr_, &data.value_.u64, getTSize(type_));
        return true;
    }
    return false;
}

bool TaskMapper::setValue(std::string data) {
    if (isValue()) {
        auto d = Value(data, type_);
        if (!d) return false;
        memcpy(ptr_, &d.value_.u64, getTSize(type_));
        return true;
    }
    return false;
}

void TaskMapper::setReferencePtr(void* ptr) {
    ptr_ = ptr;
    char* p = (char*)ptr;
    switch (struct_type_) {
    case array:
        for (auto& i : vector_) {
            i.setReferencePtr(p);
            p += i.size_;
        }
        break;
    case map:
        for (auto& i : vecmap_) {
            i.second.setReferencePtr(p);
            p += i.second.size_;
        }
        break;
    default:
        break;
    }
}

/*
 *
 *
 *
 */

const TaskMapper* KSDModule::getBranch(const std::string& prop_path) const {
    const std::regex separator("[^/]+");
    const std::regex int_exp("[ ]*(\\+|\\-)?([0-9]+)");

    auto begin = std::sregex_iterator(prop_path.begin(), prop_path.end(), separator);
    auto end = std::sregex_iterator();

    const HierarchicalData_ifs* tree = &field_map_;
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::string match_str = i->str();
        tree =
            std::regex_match(match_str, int_exp) ? tree->getArrayUint(stoul(match_str)) : tree->getMapUint(match_str);
        if (tree == nullptr) return nullptr;
    }
    return (const TaskMapper*)tree;
}

std::string printPropertys_(const TaskMapper* h_data, const std::string& indent) {
    std::string res;
    if (h_data->isMap()) {
        auto array = h_data->getMap();
        res = "\n";
        for (auto& i : array) {
            if (i.second == nullptr) return res;
            res += indent + i.first + ": " + printPropertys_((TaskMapper*)i.second, indent + "  ");
        }
    }
    if (h_data->isArray()) {
        auto array = h_data->getArray();
        int k = 0;
        res = "\n";
        for (auto& i : array) {
            res += indent + "[" + std::to_string(k) + "] :" + printPropertys_((TaskMapper*)i, indent + "  ");
        }
    }
    if (h_data->isValue()) {
        res = h_data->getValue().asString() + "\n";
    }
    return res;
}

std::string KSDModule::printProperties(const std::string& indent) const {
    const TaskMapper* h_data = &field_map_;
    return printPropertys_(h_data, indent + "  ");
}

ResValue KSDModule::getProperty(const std::string& prop_path) const {
    auto tree = getBranch(prop_path);
    if (tree->isValue()) return ResValue(tree->getValue());
    return ResValue();
}

std::string KSDModule::getPropertyAsTxt(const std::string& prop_path) const {
    const ResValue res = getProperty(prop_path);
    return asString(res.value_, res.type_);
}

bool KSDModule::setProperty(const std::string& prop_path, const Value value) {
    auto tree = (TaskMapper*)getBranch(prop_path);
    return tree->setValue(value);
}

bool KSDModule::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    auto tree = (TaskMapper*)getBranch(prop_path);
    return tree->setValue(value);
}

const TaskMapper KSDModule::header_map_ =  //
    TaskMapper({{"size", u32},
                {"id", u32},
                {"sub", u8},
                {"slot", u8},
                {"version", u16},
                {"checkSum", u16},
                {"flag", u16},
                {"dimension", u16},
                {"syncMode", u16},
                {"reserved", {12, u8}}});
