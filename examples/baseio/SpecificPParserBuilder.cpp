#include "SpecificPParserBuilder.h"

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


Parameter_ifs *AnalogParserBuilder::parse(ExtensionManager *manager, HierarchicalData_ifs *other,
                                          HierarchicalData_ifs *header) const {
    PRINT_DEBUG_INFO("Parameters.List.Analog\n");
    return nullptr;
}

Parameter_ifs *AnalogVibroParserBuilder::parse(ExtensionManager *manager, HierarchicalData_ifs *other,
                                               HierarchicalData_ifs *header) const {
    auto u = other->getMapUnit("Type");

    if (u) {
        auto vibro_type = u->getValue().value_.i64;
        if (vibro_type ==0 )




    } else
        std::cout << "cant find field type in Base\n";

    u->getValue()

        PRINT_DEBUG_INFO("Parameters.List.Analog.Vibro\n");
    return nullptr;
}

Parameter_ifs *AnalogVoltageParserBuilder::parse(ExtensionManager *manager, HierarchicalData_ifs *other,
                                                 HierarchicalData_ifs *header) const {
    PRINT_DEBUG_INFO("Parameters.List.Analog.Voltage\n");
    return nullptr;
}
