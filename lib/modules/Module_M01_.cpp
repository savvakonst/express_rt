

#include "Module_M01_.h"

Module_M01_::Module_M01_()
    : KSDModule(  //
          TaskMapper(
              {{"header", header_map_},
               {"frequency", u8},
               {"reserved_a", {2, u8}},
               {"flags", u8},
               {"out",
                TaskMapper(
                    {{"flags", u32},
                     {"dst",
                      TaskMapper({{"mac", {6, u8}}, {"port", {2, u8}}, {"ip_b", {4, u8}}, {"reserved", {4, u8}}})},
                     {"lib", TaskMapper({{"mac", u8}, {"port", u8}, {"ip_b", u8}, {"reserved", u8}})}})},
               {"slave_id", u32},
               {"reserved_b", {124, u8}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_M01_::Module_M01_(const void *ptr, size_t size, DeviceBuildingContext_ifs *context) : Module_M01_() {
    if (size != getTaskSize()) {
        error_message_ = "invalid size";
    }
    task_ = *((Task *)ptr);
    return;
}

Module_M01_::~Module_M01_() {}

const DataSchema_ifs *Module_M01_::getPropertySchema() { return nullptr; }

const HierarchicalData_ifs *Module_M01_::getProperty(const std::string &prop_path) const {
    return KSDModule::getProperty(prop_path);
}

std::string Module_M01_::getPropertyAsTxt(const std::string &prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_M01_::setProperty(const std::string &prop_path, Value value) {
    return KSDModule::setProperty(prop_path, value);
}

bool Module_M01_::setPropertyAsTxt(const std::string &prop_path, const std::string &value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs *Module_M01_::createModuleStream() {
    // error_mesadge_ = "The createModuleStream function is not realised yet";
    // TODO:
    if (ethernet_stream_ == nullptr) ethernet_stream_ = new EthernetM01_Stream(this);
    return ethernet_stream_;
};

const ErrorInfo_ifs *Module_M01_::getErrorInfo(void) const { return nullptr; }

/*
 *
 *
 *
 */

EthernetM01_Stream::EthernetM01_Stream(Module_M01_ *module) : module_(module) {
    // TODO: need to choose available modules
}

void EthernetM01_Stream::readFramePeace(ModuleStreamContext_ifs *context, char *ptr, size_t size) {}
