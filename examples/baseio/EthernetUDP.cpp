//
// Created by SVK on 12.01.2022.
//

#include "EthernetUDP.h"

#include <iostream>

#include "common/ExtensionManager.h"

EthernetUdpParameter::EthernetUdpParameter(ExtensionManager* manager, const std::string& name) : Parameter_ifs(name) {
    auto unit = manager->getLastVersionExtensionUint("data_schema", "ethernet");

    if (unit && unit->ptr) {
        data_schema_ = (DataSchema_ifs*)unit->ptr;
        data_schema_->init(manager);
    }

    parameter_field_tree_ = newParameterFieldTree(data_schema_);
    std::cout << "------------\n";
    std::cout << toString(parameter_field_tree_,"")<< "\n";
    // std::cout << "------------\n";
    // std::cout << toString(data_schema_, "--") << "\n";
}

EthernetUdpParameter::~EthernetUdpParameter() {}

PrmBuffer_ifs* EthernetUdpParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string EthernetUdpParameter::getType() const { return "EthernetUdp"; }

const DataSchema_ifs* EthernetUdpParameter::getPropertiesInfoList() { return nullptr; }

const HierarchicalData_ifs* EthernetUdpParameter::getProperty(const std::string& prop_path) const { return nullptr; }

std::string EthernetUdpParameter::getPropertyAsTxt(const std::string& prop_path) const { return std::string(); }

bool EthernetUdpParameter::setProperty(const std::string& prop_path, const Value& value) { return false; }

bool EthernetUdpParameter::setPropertyAsTxt(const std::string& prop_path, const std::string& value) { return false; }

bool EthernetUdpParameter::isValid() const { return false; }

/*
'C06M'


"Control.Levels.Number": 0,
"Delay": 0,
"Formula": "x = param()\ny = x\nreturn double(y)\n",
"Method": 0,
"Module.ID": 1295396931,

"Module.Slot": 0,
"Module.Subslot": 1,

"Points.Approximation": false,
"Points.Number": 0,
"Points.Polynomial.List": [
  {
    "Index": 0,
    "Value": 0
  },
  {
    "Index": 1,
    "Value": 0
  }
],

"Points.Polynomial.Rank": 1,
"Precision2": false,
"Raw": false,
"System": "",

*/

Parameter_ifs* EthernetUdParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                              HierarchicalData_ifs* header) const {
    // std::cout << "Parameters.List.Ethernet.UDP\n";
     std::cout << "\nheader:" << toString(header, "  ") << "\n";
     std::cout << "\nother:" << toString(other, "  ") << "\n";

    auto item = new EthernetUdpParameter(manager, "pass");

    item->setProperty("name", header->getMapUnit(" Name")->getValue());
    item->setProperty("identifier", header->getMapUnit("ShortID")->getValue());
    item->setProperty("units", header->getMapUnit("Dimension")->getValue());
    item->setProperty("category", header->getMapUnit("Category")->getValue());
    item->setProperty("department", header->getMapUnit("Department")->getValue());
    item->setProperty("description", header->getMapUnit("Description")->getValue());

    // header->

    return item;
}
