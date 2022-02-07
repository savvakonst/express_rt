

#include "Module_A06_.h"

Module_A06_::Module_A06_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {10, TaskMapper({{"frequency", u8},
                                        {"reserved", {2, u8}},
                                        {"flags", u8},
                                        {"range", i32},
                                        {"current", i32},
                                        {"offset", i32}})}},
                      {"reserved", {96, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A06_::Module_A06_(const void* ptr, size_t size, ExtensionManager* context) : Module_A06_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A06_::~Module_A06_() {}

const DataSchema_ifs* Module_A06_::getPropertySchema() { return nullptr; }

const HierarchicalData_ifs* Module_A06_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_A06_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A06_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A06_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A06_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A06_::getErrorInfo(void) const { return nullptr; }
