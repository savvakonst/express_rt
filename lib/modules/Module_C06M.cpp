

#include "Module_C06M.h"

Module_C06M::Module_C06M()
    : KSDModule(  //
          TaskMapper({{"header", header_map_}, {"inputs", u8}, {"reserved", {31, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_C06M::Module_C06M(const void* ptr, size_t size, ExtensionManager* manager) : Module_C06M() {
    if (size != getTaskSize()) {
        // TODO: print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_C06M::~Module_C06M() {}

const DataSchema_ifs* Module_C06M::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_C06M::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_C06M::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_C06M::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_C06M::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_C06M::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_C06M::getErrorInfo(void) const { return nullptr; }
