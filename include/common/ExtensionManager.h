//
// Created by SVK on 13.01.2022.
//

#ifndef EXRT_EXTENSIONMANAGER_H
#define EXRT_EXTENSIONMANAGER_H

#include <list>
#include <map>
#include <set>
#include <string>

#include "Extension.h"

#ifdef DEBUG_
#    include <iostream>
#endif

class ExtensionManager {
   private:
    struct VersionCmp {
        constexpr bool operator()(const ExtensionUnit *lhs, const ExtensionUnit *rhs) const {
            return lhs->version < rhs->version;
        }
        constexpr bool operator()(const ExtensionUnit *lhs, const version_t rhs) const { return lhs->version < rhs; }
        constexpr bool operator()(const version_t lhs, const ExtensionUnit *rhs) const { return lhs < rhs->version; }
    };

    template <typename T>
    using map_t = std::map<std::string, T>;

   public:
    typedef std::set<ExtensionUnit *, VersionCmp> versionList_t;

    std::list<std::string> getAvailableExtensionTypes(std::string type) {
        std::list<std::string> ret;
        for (auto i : tree_) ret.push_back(i.first);
        return ret;
    }

    std::list<ExtensionUnit *> getLastVersionExtensionUintsByType(std::string type) {
        std::list<ExtensionUnit *> ret;
        auto item = tree_.find(type);
        if (item != tree_.end())
            for (auto &i : item->second) ret.push_back(*(i.second.begin()));
        return ret;
    }

    const std::set<ExtensionUnit *, VersionCmp> *getExtensionUintSet(std::string name,
                                                                     std::string type) {  // it is unsafe
        auto item = tree_.find(type);
        if (item != tree_.end()) {
            auto sub_item = item->second.find(name);
            if (sub_item == item->second.end()) return &sub_item->second;
        }
        return nullptr;
    }

    const ExtensionUnit *getLastVersionExtensionUint(std::string name,
                                                     std::string type) {  // it is unsafe
        auto set = getExtensionUintSet(name, type);
        if (set == nullptr) return nullptr;
        auto ret = *(set->begin());  // there must be no situations with an empty set by design.
        return ret;
    }

    void insertExtensionInfo(ExtensionInfo *arg) {
#ifdef DEBUG_
        std::cout << "description: " << arg->description << "\n";
#endif
        for (ExtensionUnit *uint = arg->units; uint->name != nullptr; uint++) {
            insertExtensionUnit(uint);
#ifdef DEBUG_
            std::cout << "name: " << uint->name << ", type: " << uint->type << ", description: " << uint->description
                      << "\n";
#endif
        }
    }

    void insertExtensionUnit(ExtensionUnit *arg) {
        std::map<std::string, std::string> p;

        auto item = tree_.find(arg->type);
        if (item != tree_.end()) {
            item->second[arg->name].insert(arg);
        } else {
            tree_[arg->type] = map_t<versionList_t>{{arg->name, {arg}}};
        }
    }

   private:
    map_t<map_t<versionList_t>> tree_;
};

[[maybe_unused]] typedef int (*initUnit_t)(ExtensionManager *);

#endif  // EXRT_EXTENSIONMANAGER_H
