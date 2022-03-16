
#include "SpecificPParserBuilder.h"

#include "Analog.h"

#ifdef DEBUG_
#    include <iostream>
#    define PRINT_DEBUG_INFO(X) std::cout << X
#else
#    define PRINT_DEBUG_INFO(X)
#endif

/*
" Name": "parameter_vibro",
"Line": 0,
"Type": 0,
"Vibro.Description": "Напряжение",
"Vibro.Length": 30,
"Vibro.Ration": 50

" Name": "parameter_vibro",
"Line": 0,
"Type": 1,
"Vibro.Description": "Ускорение",
"Vibro.Length": 30,
"Vibro.Ration": 50

" Name": "parameter_vibro",
"Line": 0,
"Type": 2,
"Vibro.Description": "Скорость",
"Vibro.Length": 30,
"Vibro.Ration": 50

" Name": "parameter_vibro",
"Line": 0,
"Type": 3,
"Vibro.Description": "Перемещение",
"Vibro.Length": 30,
"Vibro.Ration": 50

*/

Parameter_ifs* AnalogParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                          HierarchicalData_ifs* header, const std::string& path_to_module) const {
    PRINT_DEBUG_INFO(getTypeIdentifier() << "\n");

    auto path = path_to_module + "/" + getValue(other, "Line").asString();

    Parameter_ifs* item = new AnalogParameter(manager);
    PropBuilder builder = {item, header};
    builder.setData("common/path", Value(path));

    return item;
}

Parameter_ifs* AnalogVibroParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                               HierarchicalData_ifs* header, const std::string& path_to_module) const {
    PRINT_DEBUG_INFO(getTypeIdentifier() << "\n");
    auto u = other->getMapUnit("Type");

    i64_t vibro_type = -1;
    if (u) {
        vibro_type = u->getValue().value_.i64;
    } else {
        std::cout << "cant find field type in Base\n";
        return nullptr;
    }

    Parameter_ifs* item = nullptr;

    auto path = path_to_module + "/" + getValue(other, "Line").asString();

    switch (vibro_type) {
    case 0: {
        item = new AnalogParameter(manager);
        PropBuilder builder = {item, header};
        builder.setData("common/path", Value(path));
        break;
    }
    case 1: {
        item = new AccelerationParameter(manager);
        PropBuilder builder = {item, header};
        builder.setData("common/path", Value(path));
        builder.setData("conversion_factor", getValue(other, "Vibro.Ration"));
        break;
    }
    case 2: {
        item = new VelocityParameter(manager);
        PropBuilder builder = {item, header};
        builder.setData("common/path", Value(path));
        builder.setData("conversion_factor", getValue(other, "Vibro.Ration"));
        builder.setData("length", getValue(other, "Vibro.Length"));
        break;
    }
    case 3: {
        item = new DisplacementParameter(manager);
        PropBuilder builder = {item, header};
        builder.setData("common/path", Value(path));
        builder.setData("conversion_factor", getValue(other, "Vibro.Ration"));
        builder.setData("length", getValue(other, "Vibro.Length"));
        break;
    }
    default:
        break;
    }

    return item;
}

Parameter_ifs* ThermocoupleParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                                HierarchicalData_ifs* header, const std::string& path_to_module) const {
    PRINT_DEBUG_INFO(getTypeIdentifier() << "\n");

    return nullptr;
}

Parameter_ifs* ThermoResistanceParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                                    HierarchicalData_ifs* header,
                                                    const std::string& path_to_module) const {
    PRINT_DEBUG_INFO(getTypeIdentifier() << "\n");

    auto u = other->getMapUnit("Type");

    i64_t type = -1;
    if (u) {
        type = u->getValue().value_.i64;
    } else {
        std::cout << "cant find field type in Base\n";
        return nullptr;
    }

    Parameter_ifs* item = nullptr;

    auto path = path_to_module + "/" + getValue(other, "Line").asString();

    switch (type) {
    case 0: {
        item = new ThermistorResistanceParameter(manager);
        PropBuilder builder = {item, header};
        builder.setData("common/path", Value(path));
        break;
    }
    case 1: {
        item = new ThermistorTemperatureParameter(manager);
        PropBuilder builder = {item, header};
        builder.setData("common/path", Value(path));
        builder.setData("thermistor_type", getValue(other, "Vibro.Ration"));
        break;
    }
    default:
        break;
    }

    return nullptr;
}

Parameter_ifs* AnalogVoltageParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                                 HierarchicalData_ifs* header,
                                                 const std::string& path_to_module) const {
    PRINT_DEBUG_INFO(getTypeIdentifier() << "\n");
    return nullptr;
}
