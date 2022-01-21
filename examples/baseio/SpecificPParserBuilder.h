
#ifndef SPECIFICPARSERBUILDER_H
#define SPECIFICPARSERBUILDER_H

#ifdef DEBUG_
#    include <iostream>
#    define PRINT_DEBUG_INFO(X) std::cout << X
#else
#    define PRINT_DEBUG_INFO(X)
#endif

#include "extensions/PDefaultBaseIO_ifs.h"

#define ANALOG_PB 1
class AnalogParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog"; }
    int getPrmType() const override { return ANALOG_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                         HierarchicalData_ifs* header) const override {
        PRINT_DEBUG_INFO("Parameters.List.Analog\n");
        return nullptr;
    }
};

#define ANALOG_VIBRO_PB 2
class AnalogVibroParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Vibro"; }
    int getPrmType() const override { return ANALOG_VIBRO_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                         HierarchicalData_ifs* header) const override {
        PRINT_DEBUG_INFO("Parameters.List.Analog.Vibro\n");
        return nullptr;
    }
};

#define ANALOG_VOLTAGE_PB 8
class AnalogVoltageParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Voltage"; }
    int getPrmType() const override { return ANALOG_VOLTAGE_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                         HierarchicalData_ifs* header) const override {
        PRINT_DEBUG_INFO("Parameters.List.Analog.Voltage\n");
        return nullptr;
    }
};


#undef PRINT_DEBUG_INFO
#endif