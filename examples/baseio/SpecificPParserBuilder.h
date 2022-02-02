
#ifndef SPECIFICPARSERBUILDER_H
#define SPECIFICPARSERBUILDER_H

#include <iostream>

#include "extensions/PDefaultBaseIO_ifs.h"

struct PropBuilder {
    Parameter_ifs* item_;
    HierarchicalData_ifs* header_;

    void setData(const std::string& dst, const std::string& src) const {
        auto u = header_->getMapUnit(src);
        if (u) {
            if (!item_->setProperty(dst, u->getValue())) {
                std::cout << "warning in \"" << dst << "\": " << item_->getErrorMessage() << "\n";
                item_->clearErrorMessage();
            }
        } else
            std::cout << "error: item_->setProperty(\"" << dst << "\", u->getValue());\n";
    }

    [[maybe_unused]] void setCommon() const {
        setData("common/name", " Name");
        setData("common/identifier", "ShortID");
        setData("common/name", " Name");
        setData("common/identifier", "ShortID");
        setData("common/units", "Dimension");
        setData("common/category", "Category");
        setData("common/department", "Department");
        setData("common/description", "Description");
    }
};

#define ANALOG_PB 1
class AnalogParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog"; }
    int getPrmType() const override { return ANALOG_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                         HierarchicalData_ifs* header) const override;
};

#define ANALOG_VIBRO_PB 2
class AnalogVibroParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Vibro"; }
    int getPrmType() const override { return ANALOG_VIBRO_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                         HierarchicalData_ifs* header) const override;
};

#define ANALOG_VOLTAGE_PB 8
class AnalogVoltageParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Voltage"; }
    int getPrmType() const override { return ANALOG_VOLTAGE_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                         HierarchicalData_ifs* header) const override;
};

#undef PRINT_DEBUG_INFO
#endif