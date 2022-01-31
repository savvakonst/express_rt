

#include "Module_D01_.h"

Module_D01_::Module_D01_()
    : KSDModule(  //
          TaskMapper(
              {{"header", header_map_},
               {"cnl", {16, TaskMapper({{"flags", u8}, {"voltageP", i32}, {"voltageN", i32}, {"filter", i32}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_D01_::Module_D01_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_D01_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_D01_::~Module_D01_() {}

const DataSchema_ifs* Module_D01_::getPropertySchema() { return nullptr; }

const HierarchicalData_ifs* Module_D01_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_D01_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_D01_::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_D01_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_D01_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_D01_::getErrorInfo(void) const { return nullptr; }
