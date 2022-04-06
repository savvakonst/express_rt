

#include "Module_C03_.h"

Module_C03_::Module_C03_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"active", u32},
                      {"irigMode", u32},
                      {"ir",
                       {4,
                        {4, TaskMapper({{"formatNumber", u8},
                                        {"dataBits", u8},
                                        {"parity", u8},
                                        {"syncBits", u8},
                                        {"syncWord", u64},
                                        {"frameLength", u16},
                                        {"typeOfCode", u8},
                                        {"reserved", u8}})}}},
                      {"rs",
                       {12, TaskMapper({{"frequency", u32},
                                        {"dataLength", u8},
                                        {"stopLength", u8},
                                        {"parity", u8},
                                        {"typeOfCode", u8}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_C03_::Module_C03_(const void* ptr, size_t size, ExtensionManager* manager) : Module_C03_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_C03_::~Module_C03_() {}

const DataSchema_ifs* Module_C03_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_C03_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_C03_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_C03_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_C03_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_C03_::createModuleStream() {
    error_message_ = "The createModuleStream function is not realised yet";
    return nullptr;
};

const ErrorInfo_ifs* Module_C03_::getErrorInfo(void) const { return nullptr; }
