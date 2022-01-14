//
// Created by SVK on 12.01.2022.
//

#include <iostream>

#include "extensions/PDefaultBaseIO_ifs.h"

class [[maybe_unused]] EthernetUdpParameter : public Parameter_ifs {
    EthernetUdpParameter(const std::string& name) : Parameter_ifs(name){};
    ~EthernetUdpParameter(){};

    virtual PrmBuffer_ifs* createBuffer() const override {
        // TODO: to implement this function
        return nullptr;
    }

    virtual const std::string getType() const override { return "EthernetUdp"; }

    virtual const DataSchema_ifs* getPropertiesInfoList() override { return nullptr; }

    virtual const ResValue* getProperty(const std::string& prop_path) const override {}
    virtual const std::string getPropertyAsTxt(const std::string& prop_path) const override {}

    virtual bool setProperty(const std::string& prop_path, const Value value) override {}
    virtual bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override {}

    virtual const bool isValid() const override;
    bool isLocked() const { return (bool)prm_buffer_; }
};

#define ETHERNET_UDP_PB 500
class EthernetUdParserBuilder : public PDefaultBaseIO_ifs {
   public:
    std::string getTypeIdentifier() const override { return "Parameters.List.Ethernet.UDP"; }
    int getPrmType() const override { return ETHERNET_UDP_PB; }

    Parameter_ifs* Parse(HierarchicalData_ifs* header, HierarchicalData_ifs* other) const override {
        std::cout << "EthernetUDParserBuilder";
        return nullptr;
    }
};