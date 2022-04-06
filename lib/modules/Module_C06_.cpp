

#include "Module_C06_.h"

#include "TaskMapper.h"

Module_C06_::Module_C06_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_}, {"inputs", u8}, {"reserved", {31, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_C06_::Module_C06_(const void* ptr, size_t size, ExtensionManager* manager) : Module_C06_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_C06_::~Module_C06_() {}

const DataSchema_ifs* Module_C06_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_C06_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_C06_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_C06_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_C06_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_C06_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_C06_::getErrorInfo(void) const { return nullptr; }
