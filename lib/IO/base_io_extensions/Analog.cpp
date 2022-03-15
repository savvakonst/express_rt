#include <iostream>
//
#include "Analog.h"
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"

AnalogParameter::AnalogParameter(ExtensionManager* manager) {
    auto unit = manager->getLastVersionExtensionUnit("data_schema", "analog");

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

/* AccelerationParameter
 *
 *
 *
 */
AccelerationParameter::AccelerationParameter(ExtensionManager* manager) {
    auto unit = manager->getLastVersionExtensionUnit("data_schema", "acceleration");

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

/* VelocityParameter
 *
 *
 *
 */
VelocityParameter::VelocityParameter(ExtensionManager* manager) {
    auto unit = manager->getLastVersionExtensionUnit("data_schema", "velocity");

    if (unit || unit->ptr) {
        data_schema_ = (DataSchema_ifs*)unit->ptr;
        data_schema_->init(manager);
    }

    parameter_field_tree_ = newParameterFieldTree(data_schema_);
}

VelocityParameter::~VelocityParameter() = default;

PrmBuffer_ifs* VelocityParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string VelocityParameter::getType() const { return "velocity"; }

bool VelocityParameter::isValid() const { return false; }

/* DisplacementParameter
 *
 *
 *
 */
DisplacementParameter::DisplacementParameter(ExtensionManager* manager) {
    auto unit = manager->getLastVersionExtensionUnit("data_schema", "displacement");

    if (unit || unit->ptr) {
        data_schema_ = (DataSchema_ifs*)unit->ptr;
        data_schema_->init(manager);
    }

    parameter_field_tree_ = newParameterFieldTree(data_schema_);
}

DisplacementParameter::~DisplacementParameter() = default;

PrmBuffer_ifs* DisplacementParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string DisplacementParameter::getType() const { return "displacement"; }

bool DisplacementParameter::isValid() const { return false; }