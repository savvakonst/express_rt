

#include "Module_A07_.h"

Module_A07_::Module_A07_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {8, TaskMapper({{"frequency", u8},
                                       {"reserved", {2, u8}},
                                       {"flags", u8},
                                       {"range", i32},
                                       {"current", i32},
                                       {"ureserved", u32}})}},
                      {"reserved", {128, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A07_::Module_A07_(const void* ptr, size_t size, ExtensionManager* context) : Module_A07_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A07_::~Module_A07_() {}

const DataSchema_ifs* Module_A07_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_A07_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_A07_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A07_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A07_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A07_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A07_::getErrorInfo(void) const { return nullptr; }
