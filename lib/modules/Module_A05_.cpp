

#include "Module_A05_.h"

Module_A05_::Module_A05_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {9, TaskMapper({{"frequency", u8},
                                       {"reserved", {2, u8}},
                                       {"flags", u8},
                                       {"range", i32},
                                       {"ureserved", {2, u32}}})}},
                      {"reserved", {112, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A05_::Module_A05_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_A05_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A05_::~Module_A05_() {}

const DataSchema_ifs* Module_A05_::getPropertySchema() { return nullptr; }

const HierarchicalData_ifs* Module_A05_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_A05_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A05_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A05_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A05_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A05_::getErrorInfo(void) const { return nullptr; }
