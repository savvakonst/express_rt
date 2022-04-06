

#include "Module_A11_.h"

#include "TaskMapper.h"

Module_A11_::Module_A11_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {3, TaskMapper({{"frequency", u8},
                                       {"reserved", {2, u8}},
                                       {"flags", u8},
                                       {"range", i32},
                                       {"reference", u32},
                                       {"ureserved", u32}})}},
                      {"reserved", {16, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A11_::Module_A11_(const void* ptr, size_t size, ExtensionManager* manager) : Module_A11_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A11_::~Module_A11_() {}

const DataSchema_ifs* Module_A11_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_A11_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_A11_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A11_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A11_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A11_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A11_::getErrorInfo(void) const { return nullptr; }
