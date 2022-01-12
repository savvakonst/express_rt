#ifndef SPECIFICPARSERBUILDER_H
#define SPECIFICPARSERBUILDER_H

#include <iostream>

#include "extensions/PDefaultBaseIO_ifs.h"

#define ANALOG_PB 1
class AnalogParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog"; }
    int getPrmType() const override { return ANALOG_PB; }

    Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const override {
        std::cout << "AnalogParserBuilder";
        return nullptr;
    }
};

#define ANALOG_VIBRO_PB 2
class AnalogVibroParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Vibro"; }
    int getPrmType() const override { return ANALOG_VIBRO_PB; }

    Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const override { return nullptr; }
};

#define ANALOG_VOLTAGE_PB 8
class AnalogVoltageParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Voltage"; }
    int getPrmType() const override { return ANALOG_VOLTAGE_PB; }

    Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const override { return nullptr; }
};

#define ETHERNET_UDP_PB 500
class EthernetUDParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Voltage"; }
    int getPrmType() const override { return ETHERNET_UDP_PB; }

    Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const override {
        std::cout << "EthernetUDParserBuilder";
        return nullptr;
    }
};

#endif