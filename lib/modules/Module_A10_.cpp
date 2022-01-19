

#include "Module_A10_.h"

Module_A10_::Module_A10_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {18, TaskMapper({{"frequency", u8},
                                        {"frequency2", u8},
                                        {"frequency3", u8},
                                        {"flags", u8},
                                        {"range", i32},
                                        {"waves", i32},
                                        {"hysteresis", i32}})}},
                      {"reserved", {224, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A10_::Module_A10_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_A10_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A10_::~Module_A10_() {}

const DataSchema_ifs* Module_A10_::getPropertiesInfoList() { return nullptr; }

ResValue Module_A10_::getProperty(const std::string& prop_path) const { return KSDModule::getProperty(prop_path); }
std::string Module_A10_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A10_::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A10_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A10_::createModuleStream() {
    error_mesadge_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A10_::getErrorInfo(void) const { return nullptr; }
