#include <iostream>
//
#include "Analog.h"
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"

AnalogParameter::AnalogParameter(ExtensionManager* manager) { init(manager); }

AnalogParameter::~AnalogParameter() = default;

PrmBuffer_ifs* AnalogParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string AnalogParameter::getType() const { return "analog"; }

bool AnalogParameter::isValid() const { return false; }

/* AccelerationParameter
 *
 *
 *
 */
AccelerationParameter::AccelerationParameter(ExtensionManager* manager) { init(manager); }

AccelerationParameter::~AccelerationParameter() = default;

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
VelocityParameter::VelocityParameter(ExtensionManager* manager) { init(manager); }

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
DisplacementParameter::DisplacementParameter(ExtensionManager* manager) { init(manager); }

DisplacementParameter::~DisplacementParameter() = default;

PrmBuffer_ifs* DisplacementParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string DisplacementParameter::getType() const { return "displacement"; }

bool DisplacementParameter::isValid() const { return false; }

/* DisplacementParameter
 *
 *
 *
 */
ThermocoupleEmfParameter::ThermocoupleEmfParameter(ExtensionManager* manager) { init(manager); }

ThermocoupleEmfParameter::~ThermocoupleEmfParameter() = default;

PrmBuffer_ifs* ThermocoupleEmfParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string ThermocoupleEmfParameter::getType() const { return "thermocouple_emf"; }

bool ThermocoupleEmfParameter::isValid() const { return false; }

/* ThermocoupleTemperatureParameter
 *
 *
 *
 */
ThermocoupleTemperatureParameter::ThermocoupleTemperatureParameter(ExtensionManager* manager) { init(manager); }

ThermocoupleTemperatureParameter::~ThermocoupleTemperatureParameter() = default;

PrmBuffer_ifs* ThermocoupleTemperatureParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string ThermocoupleTemperatureParameter::getType() const { return "thermocouple_temperature"; }

bool ThermocoupleTemperatureParameter::isValid() const { return false; }

/* ThermistorResistanceParameter
 *
 *
 *
 */
ThermistorResistanceParameter::ThermistorResistanceParameter(ExtensionManager* manager) { init(manager); }

ThermistorResistanceParameter::~ThermistorResistanceParameter() = default;

PrmBuffer_ifs* ThermistorResistanceParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string ThermistorResistanceParameter::getType() const { return "thermistor_resistance"; }

bool ThermistorResistanceParameter::isValid() const { return false; }

/* ThermistorTemperatureParameter
 *
 *
 *
 */
ThermistorTemperatureParameter::ThermistorTemperatureParameter(ExtensionManager* manager) { init(manager); }

ThermistorTemperatureParameter::~ThermistorTemperatureParameter() = default;

PrmBuffer_ifs* ThermistorTemperatureParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string ThermistorTemperatureParameter::getType() const { return "thermistor_temperature"; }

bool ThermistorTemperatureParameter::isValid() const { return false; }