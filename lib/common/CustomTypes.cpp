
#include "common/CustomTypes.h"

#include "common/StringProcessingTools.h"

#define PAIR(X) \
    { #X, X }
static const std::map<std::string, DataType> g_str_types_map = {MAP_DEF};
#undef PAIR
#define PAIR(X) \
    { X, #X }
static const std::map<DataType, std::string> g_types_str_map = {MAP_DEF};
#undef PAIR

std::string toString(DataType type) {
    auto val = g_types_str_map.find(type);
    if (val != g_types_str_map.end()) return val->second;
    return "none_v";
}

DataType createDataType(const std::string &str) {
    auto val = g_str_types_map.find(str);
    if (val != g_str_types_map.end()) return val->second;

    if (str == "string") return DataType::str_v;
    else if (str == "long_string")
        return DataType::lstr_v;
    else if (str == "bool")
        return DataType::bool_v;
    else if (str == "int")
        return DataType::i32;

    return DataType::none_v;
}

const HierarchicalData_ifs *getBranch(const HierarchicalData_ifs *h_data, const std::string &path) {
    if (h_data == nullptr) return nullptr;
    auto path_chunks = splitPath(path);

    for (auto &i : path_chunks) {
        auto c = (i.c_str());
        if (*c >= '0' && *c < '9') h_data = h_data->getArrayUnit((size_t)std::stoi(c));
        else
            h_data = h_data->getMapUnit(i);

        if (h_data == nullptr) return nullptr;
    }
    return h_data;
}

Value getValue(const HierarchicalData_ifs *h_data, const std::string &src) {
    auto u = getBranch(h_data, src);
    if (u) return u->getValue();
    else
        return {};
}

std::string toString(const HierarchicalData_ifs *h_data, const std::string &indent) {
    std::string res;
    if (h_data == nullptr) return res;

    if (h_data->isMap()) {
        auto array = h_data->getMap();
        res = "\n";
        for (auto &i : array) {
            if (i.second == nullptr) return res;
            res += indent + "\"" + i.first + "\": " + toString(i.second, indent + "  ");
        }
    }
    if (h_data->isArray()) {
        auto array = h_data->getArray();
        int k = 0;
        res = "\n";
        for (auto &i : array) {
            res += indent + "[" + std::to_string(k) + "] :" + toString(i, indent + "  ");
        }
    }
    if (h_data->isValue()) {
        res = h_data->getValue().asString() + " (" + toString(h_data->getValue().type_) + ")\n";
    }
    return res;
}
