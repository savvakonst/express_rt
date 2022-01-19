

#include "Module_C04_.h"

Module_C04_::Module_C04_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"active", u32},
                      {"can", {6, TaskMapper({{"frequency", u32}})}},
                      {"rs",
                       {12, TaskMapper({{"frequency", u32},
                                        {"dataLength", u8},
                                        {"stopLength", u8},
                                        {"parity", u8},
                                        {"typeOfCode", u8}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_C04_::Module_C04_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_C04_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_C04_::~Module_C04_() {}

const DataSchema_ifs* Module_C04_::getPropertiesInfoList() { return nullptr; }

ResValue Module_C04_::getProperty(const std::string& prop_path) const { return KSDModule::getProperty(prop_path); }
std::string Module_C04_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_C04_::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_C04_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_C04_::createModuleStream() {
    error_mesadge_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_C04_::getErrorInfo(void) const { return nullptr; }
