

#include "Module_A02_.h"

Module_A02_::Module_A02_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {12, TaskMapper({{"frequency", u8},
                                        {"reserved", {2, u8}},
                                        {"flags", u8},
                                        {"range", i32},
                                        {"ureserved", {2, u32}}})}},
                      {"reserved", {64, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A02_::Module_A02_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_A02_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A02_::~Module_A02_() {}

const DataSchema_ifs* Module_A02_::getPropertySchema() { return nullptr; }

const HierarchicalData_ifs* Module_A02_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_A02_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A02_::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A02_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A02_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A02_::getErrorInfo(void) const { return nullptr; }
