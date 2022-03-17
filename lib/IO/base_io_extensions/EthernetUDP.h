#ifndef EXRT_ETHERNET_UDP
#define EXRT_ETHERNET_UDP

#include "convtemplate/ParameterFieldTree.h"
#include "convtemplate/StandardParameter_ifs.h"
#include "extensions/PDefaultBaseIO_ifs.h"

/*
template <class T>
Parameter_ifs *createParameter(ExtensionManager *context, const std::string &name) {
    return new T(context, name);
}

[[maybe_unused]] typedef Parameter_ifs *(*parameterConstructor_f)(ExtensionManager *context, const std::string &name);
*/

class ExtensionUnit;

class [[maybe_unused]] EthernetUdpParameter : public StandardParameter_ifs {
   public:
    EthernetUdpParameter(ExtensionManager* manager);

    ~EthernetUdpParameter() override;

    [[nodiscard]] PrmBuffer_ifs* createBuffer() const override;

    [[nodiscard]] std::string getType() const override;

    [[nodiscard]] bool isValid() const override;
};

#define ETHERNET_UDP_PB 500
class EthernetUdParserBuilder : public PDefaultBaseIO_ifs {
   public:
    [[nodiscard]] std::string getTypeIdentifier() const override { return "Parameters.List.Ethernet.UDP"; }
    [[nodiscard]] int getPrmType() const override { return ETHERNET_UDP_PB; }

    Parameter_ifs* parse(ExtensionManager* manager, HierarchicalData_ifs* other, HierarchicalData_ifs* header,
                         const std::string& path_to_module) const override;
};

#endif