
#include "Module_A01_.h"

#include "TaskMapper.h"
#include "common.h"

typedef EthernetSyncXXXX_Stream<Module_A01_> EthernetA01_Stream;

Module_A01_::Module_A01_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {32, TaskMapper({{"frequency", u8},
                                        {"reserved", {2, u8}},
                                        {"flags", u8},
                                        {"rangeH", i32},
                                        {"rangeL", i32},
                                        {"ureserved", u32}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A01_::Module_A01_(const void* ptr, size_t size, ExtensionManager* context) : Module_A01_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A01_::~Module_A01_() = default;

const DataSchema_ifs* Module_A01_::getPropertySchema() const { return nullptr; }

const HierarchicalData_ifs* Module_A01_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}

std::string Module_A01_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A01_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}

bool Module_A01_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

bool Module_A01_::isChannelAvailable(const std::string& prop_path) const {
    auto hd = getProperty("cnl/" + prop_path + "/frequency");
    if ((hd == nullptr) || !hd->isValue()) {
        // std::cout << "path not exists: header/cnl/" + prop_path + "/frequency\n";
        return false;
    }
    return hd->getValue().value_.u8 != 0xff;
}

Value Module_A01_::getChannelProperty(const std::string& channel, const std::string& type) const {
    if (type == "frequency") {
        auto hd = getProperty("cnl/" + channel + "/frequency");
        return hd ? hd->getValue() : Value(int64_t(0xff));
    } else if (type == "factor") {
        const auto& cnl = task_.cnl[std::stoi(channel)];
        auto factor = double(cnl.rangeH) / (1000000);
        factor = factor / double(cnl.rangeL == 0 ? 1 << 16 : 1 << 15);
        return Value(factor);
    } else if (type == "signed") {
        const auto& cnl = task_.cnl[std::stoi(channel)];
        return Value(cnl.rangeL != 0);
    }
    return KSDModule::getChannelProperty(channel, type);
}

ModuleStream_ifs* Module_A01_::createModuleStream() {
    if (ethernet_stream_ != nullptr) return nullptr;
    ethernet_stream_ = new EthernetA01_Stream(this);
    return ethernet_stream_;
};

const ErrorInfo_ifs* Module_A01_::getErrorInfo() const { return nullptr; }

/*
 *
 *
 *
 */
