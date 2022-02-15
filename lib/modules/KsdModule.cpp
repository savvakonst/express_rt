
#include "KsdModule.h"

#include <regex>

/*
 *
 *
 *
 *
 */

TaskMapper::TaskMapper(const TaskMapper& task_mapper)  //
    : type_(task_mapper.type_),
      struct_type_(task_mapper.struct_type_),
      vecmap_(task_mapper.vecmap_),
      vector_(task_mapper.vector_),
      ptr_(task_mapper.ptr_),
      size_(task_mapper.size_) {
    for (auto& i : vecmap_) {
        map_[i.first] = &(i.second);
    }
}

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
        map_[i.first] = &(i.second);
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

TaskMapper::getMapReturn_t TaskMapper::getMap() const {
    // std::map<std::string, HierarchicalData_ifs*> ret;
    // for (auto& i : vecmap_) {
    //     ret[i.first] = (HierarchicalData_ifs*)&i.second;
    // }
    getMapReturn_t ret;
    for (auto& i : vecmap_) {
        ret.push_back({i.first, (HierarchicalData_ifs*)&i.second});
    }
    return ret;
}

HierarchicalData_ifs* TaskMapper::getArrayUnit(size_t id) const {
    if (id >= vector_.size()) return nullptr;
    return (TaskMapper*)&(vector_[id]);
}

HierarchicalData_ifs* TaskMapper::getMapUnit(std::string id) const {
    auto map_uint = map_.find(id);
    if (map_uint != map_.end()) return map_uint->second;
    return nullptr;
}

bool TaskMapper::setValue(const Value& data) {
    if (isValue() && (data.type_ == type_)) {
        memcpy(ptr_, &data.value_.u64, getTSize(type_));
        return true;
    }
    return false;
}

bool TaskMapper::setValue(const std::string& data) {
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
    return (const TaskMapper*)::getBranch(&field_map_, prop_path);
    /*
    const std::regex separator("[^/]+");
    const std::regex int_exp("[ ]*(\\+|\\-)?([0-9]+)");

    auto begin = std::sregex_iterator(prop_path.begin(), prop_path.end(), separator);
    auto end = std::sregex_iterator();

    const HierarchicalData_ifs* tree = &field_map_;
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::string match_str = i->str();
        tree =
            std::regex_match(match_str, int_exp) ? tree->getArrayUnit(stoul(match_str)) : tree->getMapUnit(match_str);
        if (tree == nullptr) return nullptr;
    }
    return (const TaskMapper*)tree;
     */
}

std::string KSDModule::printProperties(const std::string& indent) const {
    const TaskMapper* h_data = &field_map_;
    return toString(h_data, indent + "  ");
}

const HierarchicalData_ifs* KSDModule::getProperty(const std::string& prop_path) const {
    auto tree = (TaskMapper*)getBranch(prop_path);
    // if (tree->isValue())
    return tree;
}

std::string KSDModule::getPropertyAsTxt(const std::string& prop_path) const {
    auto res = getProperty(prop_path);
    if (res->isValue()) return res->getValue().asString();
    return toString(res, "");
}

bool KSDModule::setProperty(const std::string& prop_path, const Value& value) {
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
