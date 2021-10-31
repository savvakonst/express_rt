

#include "Module_A09_.h"

Module_A09_::Module_A09_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {6, TaskMapper({{"frequency", u8},
                                       {"reserved", {2, u8}},
                                       {"flags", u8},
                                       {"range", i32},
                                       {"reference", i32},
                                       {"sinRange", i32},
                                       {"sinFreq", i32}})}},
                      {"reserved", {40, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A09_::Module_A09_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_A09_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A09_::~Module_A09_() {}

const InfoList* Module_A09_::getPropertiesInfoList() { return nullptr; }

ResValue Module_A09_::getProperty(const std::string& prop_path) const { return KSDModule::getProperty(prop_path); }
std::string Module_A09_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A09_::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A09_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A09_::createModuleStream() {
    error_mesadge_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_A09_::getErrorInfo(void) const { return nullptr; }
