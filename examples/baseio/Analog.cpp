#include <iostream>
//
#include "Analog.h"
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"

AnalogParameter::AnalogParameter(ExtensionManager* manager, const std::string& name) : StandardParameter_ifs(name) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "analog");

    if (unit && unit->ptr) {
        data_schema_ = (DataSchema_ifs*)unit->ptr;
        data_schema_->init(manager);
    }

    parameter_field_tree_ = newParameterFieldTree(data_schema_);
}

AnalogParameter::~AnalogParameter() {}

PrmBuffer_ifs* AnalogParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string AnalogParameter::getType() const { return "EthernetUdp"; }

bool AnalogParameter::isValid() const { return false; }
