

#include "Module_D02_.h"

Module_D02_::Module_D02_()
    : KSDModule(  //
          TaskMapper(
              {{"header", header_map_},
               {"cnl",
                {24, TaskMapper({{"frequency", u8}, {"reserved", {2, u8}}, {"flags", u8}, {"filter", i32}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_D02_::Module_D02_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_D02_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_D02_::~Module_D02_() {}

const DataSchema_ifs* Module_D02_::getPropertiesInfoList() { return nullptr; }

const HierarchicalData_ifs* Module_D02_::getProperty(const std::string& prop_path) const { return KSDModule::getProperty(prop_path); }
std::string Module_D02_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_D02_::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_D02_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_D02_::createModuleStream() {
    error_mesadge_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_D02_::getErrorInfo(void) const { return nullptr; }
