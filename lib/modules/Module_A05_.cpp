

#include "Module_A05_.h"

#include "common.h"
typedef EthernetSyncXXXX_Stream<Module_A05_> EthernetA05_Stream;

Module_A05_::Module_A05_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {9, TaskMapper({{"frequency", u8},
                                       {"reserved", {2, u8}},
                                       {"flags", u8},
                                       {"range", i32},
                                       {"ureserved", {2, u32}}})}},
                      {"reserved", {112, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A05_::Module_A05_(const void* ptr, size_t size, ExtensionManager* context) : Module_A05_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A05_::~Module_A05_() = default;

const DataSchema_ifs* Module_A05_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_A05_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}
std::string Module_A05_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A05_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}

bool Module_A05_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

bool Module_A05_::isChannelAvailable(const std::string& prop_path) const {
    auto hd = getProperty("cnl/" + prop_path + "/frequency");
    if ((hd == nullptr) || !hd->isValue()) {
        return false;
    }
    return hd->getValue().value_.u8 != 0xff;
}

Value Module_A05_::getChannelProperty(const std::string& channel, const std::string& type) const {
    if (type == "frequency") {
        auto hd = getProperty("cnl/" + channel + "/frequency");
        return hd ? hd->getValue() : Value(int64_t(0xff));
    }
    return KSDModule::getChannelProperty(channel, type);
}

ModuleStream_ifs* Module_A05_::createModuleStream() {
    if (ethernet_stream_ != nullptr) return nullptr;
    ethernet_stream_ = new EthernetA05_Stream(this);
    return ethernet_stream_;
};

const ErrorInfo_ifs* Module_A05_::getErrorInfo() const { return nullptr; }
