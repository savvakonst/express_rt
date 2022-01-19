#ifndef EXRT_ETHERNET_UDP
#define EXRT_ETHERNET_UDP

#include <iostream>

#include "extensions/PDefaultBaseIO_ifs.h"

/*
template <class T>
Parameter_ifs *createParameter(ExtensionManager *context, const std::string &name) {
    return new T(context, name);
}

[[maybe_unused]] typedef Parameter_ifs *(*parameterConstructor_f)(ExtensionManager *context, const std::string &name);
*/
class ExtensionUnit;

class [[maybe_unused]] EthernetUdpParameter : public Parameter_ifs {
   public:
    EthernetUdpParameter(ExtensionManager* manager, const std::string& name);

    ~EthernetUdpParameter() override;

    [[nodiscard]] PrmBuffer_ifs* createBuffer() const override;

    [[nodiscard]] std::string getType() const override;

    const DataSchema_ifs* getPropertiesInfoList() override;

    [[nodiscard]] const ResValue* getProperty(const std::string& prop_path) const override;
    [[nodiscard]] std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value& value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& value) override;

    [[nodiscard]] bool isValid() const override;

   private:
    DataSchema_ifs* data_schema_;
};

#define ETHERNET_UDP_PB 500
class EthernetUdParserBuilder : public PDefaultBaseIO_ifs {
   public:
    [[nodiscard]] std::string getTypeIdentifier() const override { return "Parameters.List.Ethernet.UDP"; }
    [[nodiscard]] int getPrmType() const override { return ETHERNET_UDP_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                         HierarchicalData_ifs* header) const override;
};

#endif