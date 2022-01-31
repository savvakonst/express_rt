
#include <regex>
//
#include "Device/Device.h"
#include "Device/ModuleStream_ifs.h"
#include "common/StringProcessingTools.h"

// TODO:
#pragma pack(1)
struct MODULE_HEADER {
    uint32_t size;
    uint32_t id;
    uint8_t sub;
    uint8_t slot;
    uint16_t version;
    uint16_t checkSum;
    uint16_t flag;
    uint16_t dimension;
    uint16_t syncMode;
    uint8_t reserved[12];
};
#pragma pack()
#include "iostream"

Device::Device(const void *ptr, size_t size, DeviceBuildingContext_ifs *context) {
    if (size < sizeof(TASK_HEADER)) {
        error_message_ = "task size is too small";
    }

    task_header_ = *(TASK_HEADER *)ptr;

    size_ = (size_t)task_header_.taskSize;
    size_t offset = sizeof(TASK_HEADER);

    do {
        char *current_ptr = (char *)ptr + offset;
        MODULE_HEADER *header = (MODULE_HEADER *)(current_ptr);
        Module_ifs *module = context->createModule(stringId(header->id), current_ptr, (size_t)header->size, context);

        if (!module) {
            error_message_ = "cant find module with \"" + stringId(header->id) + "\" id";
            return;
        }

        offset += (size_t)header->size;
        modules_.push_back(module);
    } while (offset < size_);

    if (offset != getTaskSize()) {
        error_message_ = "sum of module sizes is not equal \"taskSize\" ";
    }

    // modules_.push_back()
}

Device::~Device() {}

exo_container<const Module_ifs *> Device::getAllModules() { return exo_container<const Module_ifs *>(); }

const Module_ifs *Device::getModuleFromPath(const std::string &name) {
    static const std::regex validator(R"((([\w]+:[\w]+:[\w]+/)*)(\w+:[\w]))");

    std::smatch matches;
    if (std::regex_search(name, matches, validator)) {
        std::vector<std::string> path_chunks = splitPath(matches[1]);
        std::string terminal = matches[3];

        if (path_chunks.size()) {
            error_message_ = "module encapsulation is not supported yet";
            return nullptr;
        }
    }
    return nullptr;
}

exo_container<const Module_ifs *> Device::getLineFromPath(const std::string &path) {
    error_message_ = "module encapsulation is not supported yet";
    return exo_container<const Module_ifs *>();
}

status Device::checkValExistence(const std::string &path) { return status::failure; }

status Device::hasTransceiver() const {
    for (auto i : modules_) {
        if (i->hasTransceiver()) return status::succes;
    }
    return status::failure;
}

EthernetSettings Device::getSrcAddress() const {
    for (auto i : modules_) {
        if (i->hasTransceiver()) return i->getSrcAddress();
    }
    return {};
}

size_t Device::getTaskSize() const { return size_; }
