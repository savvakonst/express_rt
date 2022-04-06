
#include "device/KsdModule.h"

#include <regex>

/*
 *
 *
 *
 */

std::string KSDModule::getModulePath(bool full_path) const {
    auto slot = getProperty("header/slot")->getValue().value_.u64;
    auto sub_slot = getProperty("header/sub")->getValue().value_.u64;
    auto path = std::to_string(slot) + ((sub_slot == 0) ? "" : "." + std::to_string(sub_slot)) + "/" + getID();

    if (parent_module_ && full_path) path = parent_module_->getModulePath(true) + "/" + path;
    return path;
}

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

bool KSDModule::isChannelAvailable(const std::string& prop_path) const { return false; }

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
