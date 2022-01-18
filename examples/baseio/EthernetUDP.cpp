//
// Created by SVK on 12.01.2022.
//

#include "EthernetUDP.h"

#include <iostream>

#include "common/ExtensionManager.h"

EthernetUdpParameter::EthernetUdpParameter(ExtensionManager* manager, const std::string& name) : Parameter_ifs(name) {
    auto ext = manager->getLastVersionExtensionUint("data_schema", "ethernet");
}

EthernetUdpParameter::~EthernetUdpParameter() {}

PrmBuffer_ifs* EthernetUdpParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string EthernetUdpParameter::getType() const { return "EthernetUdp"; }

const DataSchema_ifs* EthernetUdpParameter::getPropertiesInfoList() { return nullptr; }

const ResValue* EthernetUdpParameter::getProperty(const std::string& prop_path) const { return nullptr; }

std::string EthernetUdpParameter::getPropertyAsTxt(const std::string& prop_path) const { return std::string(); }

bool EthernetUdpParameter::setProperty(const std::string& prop_path, const Value& value) { return false; }

bool EthernetUdpParameter::setPropertyAsTxt(const std::string& prop_path, const std::string& value) { return false; }

bool EthernetUdpParameter::isValid() const { return false; }

Parameter_ifs* EthernetUdParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                              HierarchicalData_ifs* header) const {
    std::cout << "Parameters.List.Ethernet.UDP\n";
    std::cout << toString(header, "  ") << "\n";
    auto item = new EthernetUdpParameter(manager, "pass");
    return item;
}
