
#ifndef SPECIFICPARSERBUILDER_H
#define SPECIFICPARSERBUILDER_H

#include <iostream>

#include "device/Module_ifs.h"
#include "extensions/PDefaultBaseIO_ifs.h"

struct PropBuilder {
    Parameter_ifs* item_;
    HierarchicalData_ifs* header_;

    PropBuilder(Parameter_ifs* item, HierarchicalData_ifs* header) : item_(item), header_(header) { setCommon(); }

    /**
     * gets data from a field named src and write it in to a field named dst
     * @param dst
     * @param src
     */
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

    void setData(const std::string& dst, const Value& value) const {
        if (!item_->setProperty(dst, value)) {
            std::cout << "warning in \"" << dst << "\": " << item_->getErrorMessage() << "\n";
            item_->clearErrorMessage();
        }
    }

    [[maybe_unused]] void setCommon() const {
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
    [[nodiscard]] std::string getTypeIdentifier() const override { return "Parameters.List.Analog"; }
    [[nodiscard]] int getPrmType() const override { return ANALOG_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other, HierarchicalData_ifs* header,
                         const std::string& path_to_module) const override;
};

#define ANALOG_VIBRO_PB 2
class AnalogVibroParserBuilder : public PDefaultBaseIO_ifs {
   public:
    [[nodiscard]] std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Vibro"; }
    [[nodiscard]] int getPrmType() const override { return ANALOG_VIBRO_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other, HierarchicalData_ifs* header,
                         const std::string& path_to_module) const override;
};

#define THERMOCOUPLE_PB 3
class ThermocoupleParserBuilder : public PDefaultBaseIO_ifs {
   public:
    [[nodiscard]] std::string getTypeIdentifier() const override { return "Parameters.List.Analog.TermoCouple"; }
    [[nodiscard]] int getPrmType() const override { return THERMOCOUPLE_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other, HierarchicalData_ifs* header,
                         const std::string& path_to_module) const override;
};

#define THERMORESISTANCE_PB 4
class ThermoResistanceParserBuilder : public PDefaultBaseIO_ifs {
   public:
    [[nodiscard]] std::string getTypeIdentifier() const override { return "Parameters.List.Analog.TermResistance"; }
    [[nodiscard]] int getPrmType() const override { return THERMORESISTANCE_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other, HierarchicalData_ifs* header,
                         const std::string& path_to_module) const override;
};

#define ANALOG_VOLTAGE_PB 8
class AnalogVoltageParserBuilder : public PDefaultBaseIO_ifs {
   public:
    [[nodiscard]] std::string getTypeIdentifier() const override { return "Parameters.List.Analog.Voltage"; }
    [[nodiscard]] int getPrmType() const override { return ANALOG_VOLTAGE_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other, HierarchicalData_ifs* header,
                         const std::string& path_to_module) const override;
};

#undef PRINT_DEBUG_INFO
#endif