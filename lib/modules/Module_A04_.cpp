

#include "Module_A04_.h"

#include "common.h"

typedef EthernetSyncXXXX_Stream<Module_A04_> EthernetA04_Stream;

Module_A04_::Module_A04_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {12, TaskMapper({{"frequency", u8},
                                        {"reserved", {2, u8}},
                                        {"flags", u8},
                                        {"range", i32},
                                        {"ureserved", {2, u32}}})}},
                      {"reserved", {64, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A04_::Module_A04_(const void* ptr, size_t size, ExtensionManager* context) : Module_A04_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A04_::~Module_A04_() {}

const DataSchema_ifs* Module_A04_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_A04_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_A04_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A04_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A04_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

bool Module_A04_::isChannelAvailable(const std::string& prop_path) const {
    if (!isAvailable()) return false;
    auto hd = getProperty("cnl/" + prop_path + "/frequency");
    if ((hd == nullptr) || !hd->isValue()) {
        return false;
    }
    return hd->getValue().value_.u8 != 0xff;
}

Value Module_A04_::getChannelProperty(const std::string& channel, const std::string& type) const {
    if (type == "frequency") {
        auto hd = getProperty("cnl/" + channel + "/frequency");
        return hd ? hd->getValue() : Value(int64_t(0xff));
    }
    return KSDModule::getChannelProperty(channel, type);
}

ModuleStream_ifs* Module_A04_::createModuleStream() {
    if (ethernet_stream_ != nullptr) return nullptr;
    ethernet_stream_ = new EthernetA04_Stream(this);
    return ethernet_stream_;
};

const ErrorInfo_ifs* Module_A04_::getErrorInfo(void) const { return nullptr; }
