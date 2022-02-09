#include <iostream>
//
#include "Analog.h"
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"

AnalogParameter::AnalogParameter(ExtensionManager* manager) {
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

/*
 *
 *
 *
 */
AccelerationParameter::AccelerationParameter(ExtensionManager* manager) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "acceleration");

    if (unit || unit->ptr) {
        data_schema_ = (DataSchema_ifs*)unit->ptr;
        data_schema_->init(manager);
    }

    parameter_field_tree_ = newParameterFieldTree(data_schema_);
}

AccelerationParameter::~AccelerationParameter() {}

PrmBuffer_ifs* AccelerationParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string AccelerationParameter::getType() const { return "acceleration"; }

bool AccelerationParameter::isValid() const { return false; }
