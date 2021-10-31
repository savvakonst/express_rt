#ifndef SPECIFICPARSERBUILDER_H
#define SPECIFICPARSERBUILDER_H

#include "extensions/PDefaultBaseIO_ifs.h"

#define ANALOG 1
class AnalogParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() { return "Parameters.List.Analog"; }
    int getPrmType() { return ANALOG; }

    Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const override { return nullptr; }
};

#define ANALOG_VIBRO 2
class AnalogVibroParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() { return "Parameters.List.Analog.Vibro"; }
    int getPrmType() { return ANALOG_VIBRO; }

    Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const override { return nullptr; }
};

#define ANALOG_VOLTAGE 8
class AnalogVoltageParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() { return "Parameters.List.Analog.Voltage"; }
    int getPrmType() { return ANALOG_VOLTAGE; }

    Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const override { return nullptr; }
};

#endif