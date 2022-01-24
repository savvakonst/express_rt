

#include "Module_ID02.h"

Module_ID02::Module_ID02()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"out",
                       {2, TaskMapper({{"state", u8},
                                       {"reservedA", u8},
                                       {"onCode", u16},
                                       {"offCode", u16},
                                       {"width", u16},
                                       {"frequency", u16},
                                       {"record", u8},
                                       {"reservedB", u8}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_ID02::Module_ID02(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_ID02() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_ID02::~Module_ID02() {}

const DataSchema_ifs* Module_ID02::getPropertiesInfoList() { return nullptr; }

const HierarchicalData_ifs* Module_ID02::getProperty(const std::string& prop_path) const { return KSDModule::getProperty(prop_path); }
std::string Module_ID02::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_ID02::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_ID02::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_ID02::createModuleStream() {
    error_mesadge_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_ID02::getErrorInfo(void) const { return nullptr; }
