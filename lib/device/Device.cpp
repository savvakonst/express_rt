
#include <regex>
//
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"
#include "device/Device.h"
#include "device/ModuleStream_ifs.h"

// TODO:
#pragma pack(1)
struct MODULE_HEADER {
    uint32_t size;
    uint32_t id;
    uint8_t sub;
    uint8_t slot;
    uint16_t version;
    uint16_t check_sum;
    uint16_t flag;
    uint16_t dimension;
    uint16_t sync_mode;
    uint8_t reserved[12];
};
#pragma pack()

Device::Device(const void *ptr, size_t size, ExtensionManager *context) {
    auto manager = (ExtensionManager *)context;

    if (size < sizeof(TaskHeader)) {
        error_message_ = "task size is too small";
    }

    task_header_ = *(TaskHeader *)ptr;

    size_ = (size_t)task_header_.task_size;
    size_t offset = sizeof(TaskHeader);

    do {
        char *current_ptr = (char *)ptr + offset;
        auto *header = (MODULE_HEADER *)(current_ptr);

        auto constructor = (moduleConstructor_f)manager->getLastVersionExtensionObject("module", stringId(header->id));
        Module_ifs *module = constructor(current_ptr, (size_t)header->size, manager);

        if (!module) {
            error_message_ = "cant find module with \"" + stringId(header->id) + "\" id";
            return;
        }
        module->setParentModule(this);
        offset += (size_t)header->size;
        modules_.push_back(module);
    } while (offset < size_);

    if (offset != size_) {
        error_message_ = "sum of module sizes is not equal \"taskSize\" ";
    }

    // modules_.push_back()
}

Device::~Device() = default;

bool Device::hasTransceiver() const {
    for (auto i : modules_) {
        if (i->hasTransceiver()) return true;
    }
    return false;
}

EthernetSettings Device::getSrcAddress() const {
    for (auto i : modules_) {
        if (i->hasTransceiver()) return i->getSrcAddress();
    }
    return {};
}

std::string Device::getID() const { return stringId(task_header_.device_id); }

std::list<Module_ifs *> Device::getSubModulesFromPath(const std::string &modules_path) const {
    std::list<Module_ifs *> ret_list;
    for (auto i : modules_) ret_list.splice(ret_list.cend(), ::getSubmodules(i, modules_path));

    return ret_list;
}

std::vector<std::pair<std::string, Module_ifs *>> Device::getSubModules() const {
    std::vector<std::pair<std::string, Module_ifs *>> ret;

    ret.reserve(modules_.size());
    for (auto i : modules_) {
        auto slot = i->getProperty("header/slot")->getValue().value_.u64;
        auto sub_slot = i->getProperty("header/sub")->getValue().value_.u64;
        auto path = std::to_string(slot) + ((sub_slot == 0) ? "" : "." + std::to_string(sub_slot));

        ret.push_back({path, i});
    }

    return ret;
}

size_t Device::getTaskSize() const { return size_; }

/*
 *
 *
 *
 */

#define PATHSEP '/'
std::list<Module_ifs *> getSubmodules(Module_ifs *i, const std::string &glob_pattern) {
    std::list<Module_ifs *> ret_list;

    auto local_path = i->getModulePath(false);
    auto text = local_path.c_str();
    auto glob = glob_pattern.c_str();

    const char *text_backup = nullptr;
    const char *glob_backup = nullptr;
    const char *text_2_star_backup = nullptr;
    const char *glob_2_star_backup = nullptr;

    if (*glob == '/') {
        while (*text == '.' && text[1] == PATHSEP) text += 2;
        if (*text == PATHSEP) text++;
        glob++;
    } else if (strchr(glob, '/') == nullptr) {
        const char *sep = strrchr(text, PATHSEP);
        if (sep) text = sep + 1;
    }

    while (*text != '\0') {
        switch (*glob) {
        case '*': {
            if (*++glob == '*') {
                // invalid syntax

                if (*++glob == '\0') goto ret_true;

                if (*glob != '/') return {};
                ret_list.splice(ret_list.cend(), i->getSubModulesFromPath(glob - 2));

                text_backup = nullptr;
                glob_backup = nullptr;
                text_2_star_backup = text;
                glob_2_star_backup = ++glob;
                continue;
            }
            // trailing * matches everything except /
            text_backup = text;
            glob_backup = glob;
            continue;
        }
        case '?':
            if (*text == PATHSEP) break;
            text++;
            glob++;
            continue;
        case '\\':
            glob++;
        default:
            if (*glob != *text && !(*glob == '/' && *text == PATHSEP)) break;
            text++;
            glob++;
            continue;
        }
        if (glob_backup != nullptr && *text_backup != PATHSEP) {
            // *-loop: backtrack to the last * but do not jump over /
            text = ++text_backup;
            glob = glob_backup;
            continue;
        }
        if (glob_2_star_backup != nullptr) {
            // **-loop: backtrack to the last **
            text = ++text_2_star_backup;
            glob = glob_2_star_backup;
            continue;
        }
        goto ret_false;
    }
ret_true:
    if (*glob != '\0') ret_list.splice(ret_list.cend(), i->getSubModulesFromPath(glob));
    else
        ret_list.push_back(i);
ret_false:;
    return ret_list;
}
