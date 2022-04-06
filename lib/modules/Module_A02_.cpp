
#include "Module_A02_.h"

#include "TaskMapper.h"
#include "common.h"

typedef EthernetSyncXXXX_Stream<Module_A02_> EthernetA02_Stream;

Module_A02_::Module_A02_()
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

Module_A02_::Module_A02_(const void* ptr, size_t size, ExtensionManager* context) : Module_A02_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A02_::~Module_A02_() = default;

const DataSchema_ifs* Module_A02_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_A02_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}

std::string Module_A02_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A02_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}

bool Module_A02_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

bool Module_A02_::isChannelAvailable(const std::string& prop_path) const {
    auto hd = getProperty("cnl/" + prop_path + "/frequency");
    if ((hd == nullptr) || !hd->isValue()) {
        return false;
    }
    return hd->getValue().value_.u8 != 0xff;
}

Value Module_A02_::getChannelProperty(const std::string& channel, const std::string& type) const {
    if (type == "frequency") {
        auto hd = getProperty("cnl/" + channel + "/frequency");
        return hd ? hd->getValue() : Value(int64_t(0xff));
    } else if (type == "factor") {
        const auto& cnl = task_.cnl[std::stoi(channel)];
        auto factor = double(cnl.range) / (1000000);
        factor = factor / (1 << 15);
        return Value(factor);
    } else if (type == "signed") {
        return Value(true);
    }
    return KSDModule::getChannelProperty(channel, type);
}

ModuleStream_ifs* Module_A02_::createModuleStream() {
    if (ethernet_stream_ != nullptr) return nullptr;
    ethernet_stream_ = new EthernetA02_Stream(this);
    return ethernet_stream_;
};

const ErrorInfo_ifs* Module_A02_::getErrorInfo() const { return nullptr; }

/*
 *
 *
 *
 */