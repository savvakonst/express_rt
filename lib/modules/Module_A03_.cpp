

#include "Module_A03_.h"

Module_A03_::Module_A03_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {16, TaskMapper({{"frequency", u8},
                                        {"reserved", {2, u8}},
                                        {"flags", u8},
                                        {"range", i32},
                                        {"current", i32},
                                        {"offset", i32}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A03_::Module_A03_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_A03_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A03_::~Module_A03_() {}

const DataSchema_ifs* Module_A03_::getPropertySchema() { return nullptr; }

const HierarchicalData_ifs* Module_A03_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_A03_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A03_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A03_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A03_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A03_::getErrorInfo(void) const { return nullptr; }
