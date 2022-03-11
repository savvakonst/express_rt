
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
    PRINT_DEBUG_INFO("Parameters.List.Analog\n");

    Parameter_ifs* item = new AnalogParameter(manager);
    PropBuilder builder = {item, header};
    builder.setData("common/path", Value(path_to_module));

    return item;
}

Parameter_ifs* AnalogVibroParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                               HierarchicalData_ifs* header, const std::string& path_to_module) const {
    PRINT_DEBUG_INFO("Parameters.List.Analog.Vibro\n");
    auto u = other->getMapUnit("Type");

    i64_t vibro_type = -1;
    if (u) {
        vibro_type = u->getValue().value_.i64;
    } else {
        std::cout << "cant find field type in Base\n";
        return nullptr;
    }

    Parameter_ifs* item = nullptr;
    switch (vibro_type) {
    case 0: {
        item = new AnalogParameter(manager);
        PropBuilder builder = {item, header};
        builder.setData("common/path", Value(path_to_module));

        break;
    }
    case 1: {
        item = new AccelerationParameter(manager);
        PropBuilder builder = {item, header};
        builder.setData("common/path", Value(path_to_module));
        builder.setData("conversion_factor", Value(1.0));
        break;
    }
    case 2:
        break;
    case 3:
        break;
    }

    return item;
}

Parameter_ifs* AnalogVoltageParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                                 HierarchicalData_ifs* header,
                                                 const std::string& path_to_module) const {
    PRINT_DEBUG_INFO("Parameters.List.Analog.Voltage\n");
    return nullptr;
}
