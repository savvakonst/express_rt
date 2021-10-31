

#include "Module_M01_.h"

Module_M01_::Module_M01_()
    : KSDModule(  //
          TaskMapper(
              {{"header", i32},
               {"frequency", u8},
               {"reserved_a", {2, u8}},
               {"flags", u8},
               {"out",
                TaskMapper(
                    {{"flags", u32},
                     {"dst",
                      TaskMapper({{"mac", {6, u8}}, {"port", {2, u8}}, {"ip_b", {4, u8}}, {"reserved", {4, u8}}})},
                     {"src", TaskMapper({{"mac", u8}, {"port", u8}, {"ip_b", u8}, {"reserved", u8}})}})},
               {"slave_id", u32},
               {"reserved_b", {124, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_M01_::Module_M01_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_M01_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_M01_::~Module_M01_() {}

const InfoList* Module_M01_::getPropertiesInfoList() { return nullptr; }

ResValue Module_M01_::getProperty(const std::string& prop_path) const { return KSDModule::getProperty(prop_path); }
std::string Module_M01_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_M01_::setProperty(const std::string& prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_M01_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_M01_::createModuleStream() {
    error_mesadge_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_M01_::getErrorInfo(void) const { return nullptr; }
