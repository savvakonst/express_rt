
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

std::vector<std::pair<std::string, Module_ifs *>> Device::getSubModules() const {
    std::vector<std::pair<std::string, Module_ifs *>> ret;
    // ret.resize(number_of_slots_,{"", nullptr});
    // ret.reserve(number_of_slots_);

    ret.reserve(modules_.size());
    for (auto i : modules_) {
        /*auto slot = i->getProperty("header/slot")->getValue().value_.u64;
        auto sub_slot = i->getProperty("header/sub")->getValue().value_.u64;
        auto path = std::to_string(slot) + ((sub_slot == 0) ? "" : "." + std::to_string(sub_slot));
         */
        ret.push_back({"", i});
    }

    return ret;
}

std::vector<std::pair<std::string, Module_ifs *>> Device::getModulesFromPath(const std::string &name) {
    static const std::regex validator(R"((([\w]+:[\w]+:[\w]+/)*)(\w+:[\w]))");

    std::smatch matches;
    if (std::regex_search(name, matches, validator)) {
        std::vector<std::string> path_chunks = splitPath(matches[1]);
        std::string terminal = matches[3];

        if (!path_chunks.empty()) {
            error_message_ = "module encapsulation is not supported yet";
            return {};
        }
    }
    return {};
}

size_t Device::getTaskSize() const { return size_; }
