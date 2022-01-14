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

class ExtensionManager {
   private:
    struct VersionCmp {
        constexpr bool operator()(const ExtensionUint *lhs, const ExtensionUint *rhs) const {
            return lhs->version < rhs->version;
        }
        constexpr bool operator()(const ExtensionUint *lhs, const version_t rhs) const { return lhs->version < rhs; }
        constexpr bool operator()(const version_t lhs, const ExtensionUint *rhs) const { return lhs < rhs->version; }
    };

    template <typename T>
    using map_t = std::map<std::string, T>;

   public:
    typedef std::set<ExtensionUint *, VersionCmp> versionList_t;

    std::list<std::string> getAvailableExtensionTypes(std::string type) {
        std::list<std::string> ret;
        for (auto i : tree_) ret.push_back(i.first);
        return ret;
    }

    std::list<ExtensionUint *> getLastVersionExtensionUintsByType(std::string type) {
        std::list<ExtensionUint *> ret;
        auto item = tree_.find(type);
        if (item != tree_.end())
            for (auto &i : item->second) ret.push_back(*(i.second.begin()));
        return ret;
    }

    const std::set<ExtensionUint *, VersionCmp> *getExtensionUintSet(std::string name,
                                                                     std::string type) {  // it is unsafe
        auto item = tree_.find(type);
        if (item != tree_.end()) {
            auto sub_item = item->second.find(name);
            if (sub_item == item->second.end()) return &sub_item->second;
        }
        return nullptr;
    }

    const ExtensionUint *getLastVersionExtensionUint(std::string name,
                                                     std::string type) {  // it is unsafe
        auto set = getExtensionUintSet(name, type);
        if (set == nullptr) return nullptr;
        auto ret = *(set->begin());  // there must be no situations with an empty set by design.
        return ret;
    }

    void insertExtensionInfo(ExtensionInfo *arg) {
        // std::cout << "description: " << arg->description << "\n";
        for (ExtensionUint *uint = arg->uints; uint->name != nullptr; uint++) {
            insertExtensionUint(uint);
            // std::cout << "name: " << uint->name << ", type: " << uint->type << ", description: " << uint->description
            //           << "\n";
        }
    }

    void insertExtensionUint(ExtensionUint *arg) {
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

#endif  // EXRT_EXTENSIONMANAGER_H
