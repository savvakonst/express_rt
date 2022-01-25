//
// Created by SVK on 12.01.2022.
//

#include "EthernetUDP.h"

#include <iostream>

#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"

EthernetUdpParameter::EthernetUdpParameter(ExtensionManager* manager, const std::string& name)
    : StandardParameter_ifs(name) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "ethernet");

    if (unit && unit->ptr) {
        data_schema_ = (DataSchema_ifs*)unit->ptr;
        data_schema_->init(manager);
    }

    parameter_field_tree_ = newParameterFieldTree(data_schema_);
    std::cout << "------------\n";
    // std::cout << toString(parameter_field_tree_,"")<< "\n";
    std::cout << toString(getBranch(parameter_field_tree_, "destination"), "") << "\n";
    // std::cout << "------------\n";
    // std::cout << toString(data_schema_, "--") << "\n";
}

EthernetUdpParameter::~EthernetUdpParameter() {}

PrmBuffer_ifs* EthernetUdpParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string EthernetUdpParameter::getType() const { return "EthernetUdp"; }

bool EthernetUdpParameter::isValid() const { return false; }

Parameter_ifs* EthernetUdParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                              HierarchicalData_ifs* header) const {
    // std::cout << "Parameters.List.Ethernet.UDP\n";
    // std::cout << "\nheader:" << toString(header, "  ") << "\n";
    // std::cout << "\nother:" << toString(other, "  ") << "\n";

    auto item = new EthernetUdpParameter(manager, "pass");

    item->setProperty("common/name", header->getMapUnit(" Name")->getValue());
    item->setProperty("common/identifier", header->getMapUnit("ShortID")->getValue());
    item->setProperty("common/units", header->getMapUnit("Dimension")->getValue());
    item->setProperty("common/category", header->getMapUnit("Category")->getValue());
    item->setProperty("common/department", header->getMapUnit("Department")->getValue());
    item->setProperty("common/description", header->getMapUnit("Description")->getValue());

    item->setPropertyAsTxt("destination/ip", "190");
    std::cout << toString(item->getProperty(""), "") << "\n";
    return item;
}
