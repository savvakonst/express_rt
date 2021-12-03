

#include "Device/Device.h"

#include <regex>

#include "Device/ModuleStream_ifs.h"


Device::Device(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) {
    if (size < sizeof(TASK_HEADER)) {
        error_mesadge_ = "task size is too small";
    }

    task_header_ = *(TASK_HEADER*)ptr;

}

Device::~Device() {}

exo_container<const Module_ifs*> Device::getAllModules() { return exo_container<const Module_ifs*>(); }

std::vector<std::string> splitPath(const std::string& path) {
    static const std::regex separator("[^/]+");
    std::vector<std::string> ret;
    ret.reserve(2);

    auto begin = std::sregex_iterator(path.begin(), path.end(), separator);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i) ret.push_back(i->str());

    return ret;
}

const Module_ifs* Device::getModuleFromPath(std::string path) {
    static const std::regex validator("(([\\w]+:[\\w]+:[\\w]+/)*)(\\w+:[\\w])");

    std::smatch matches;
    if (std::regex_search(path, matches, validator)) {
        std::vector<std::string> path_chunks = splitPath(matches[1]);
        std::string terminal = matches[3];

        if (path_chunks.size()) {
            error_mesadge_ = "module incapsulation is not supported yet";
            return nullptr;
        }
    }
    return nullptr;
}

exo_container<const Module_ifs*> Device::getLineFromPath(std::string path) {
    error_mesadge_ = "module incapsulation is not supported yet";
    return exo_container<const Module_ifs*>();
}

status Device::checkValExistence(std::string path) { return status::failure; }

status Device::isTranciverEnabled() { return status::failure; }

std::string Device::getSrcAddress() { return ""; }
