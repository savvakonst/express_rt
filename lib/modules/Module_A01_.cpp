

#include "Module_A01_.h"

Module_A01_::Module_A01_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {32, TaskMapper({{"frequency", u8},
                                        {"reserved", {2, u8}},
                                        {"flags", u8},
                                        {"rangeH", i32},
                                        {"rangeL", i32},
                                        {"ureserved", u32}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A01_::Module_A01_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_A01_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A01_::~Module_A01_() {}

const InfoList* Module_A01_::getPropertiesInfoList() { return nullptr; }

ResValue Module_A01_::getProperty(const std::string& prop_path) const { return KSDModule::getProperty(prop_path); }
std::string Module_A01_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A01_::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A01_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A01_::createModuleStream() {
    error_mesadge_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A01_::getErrorInfo(void) const { return nullptr; }
