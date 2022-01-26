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

struct PropBuilder {
    Parameter_ifs* item_;
    HierarchicalData_ifs* header_;

    void setData(const std::string& dst, const std::string src) {
        auto u = header_->getMapUnit(src);
        if (u) {
            if (!item_->setProperty(dst, u->getValue())) {
                std::cout << "warning in \"" << dst << "\": " << item_->getErrorMessage() << "\n";
                item_->clearErrorMessage();
            }
        } else
            std::cout << "error: item_->setProperty(\"" << dst << "\", u->getValue());\n";
    }
};

Parameter_ifs* EthernetUdParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                              HierarchicalData_ifs* header) const {
    // std::cout << "Parameters.List.Ethernet.UDP\n";
    //std::cout << "\nheader:" << toString(header, "  ") << "\n";
    //std::cout << "\nother:" << toString(other, "  ") << "\n";

    auto item = new EthernetUdpParameter(manager, "pass");
    PropBuilder builder = {item, header};

    builder.setData("common/name", " Name");
    builder.setData("common/identifier", "ShortID");
    builder.setData("common/name", " Name");
    builder.setData("common/identifier", "ShortID");
    builder.setData("common/units", "Dimension");
    builder.setData("common/category", "Category");
    builder.setData("common/department", "Department");
    builder.setData("common/description", "Description");

    builder.header_ = other;

    builder.setData("source/ip", "Src.IP");
    builder.setData("source/port", "Src.Port");
    builder.setData("destination/ip", "Dst.IP");
    builder.setData("destination/port", "Dst.Port");


    static const std::map<int64_t, std::string> type_mapper{{1, "i8"},  {2, "i16"},    {3, "i32"},
                                                            {4, "i64"}, {0x24, "f32"}, {0x28, "f64"}};

    auto u = other->getMapUnit("Data.Type");
    if (u) {
        auto it = type_mapper.find(u->getValue().value_.i64);
        if (it == type_mapper.end()) {
            std::cout << R"(error: cant map "Data.Type" field to corresponding field "type")";
        } else if (!item->setProperty("type", Value(it->second))) {
            std::cout << "error: " << item->getErrorMessage() << "\n";
            item->clearErrorMessage();
        }
    } else
        std::cout << "error: item_->setProperty(\"type\", u->getValue());\n";

    u = other->getMapUnit("Data.Endian");
    if (u) {
        auto it = u->getValue().value_.i64;
        auto v = Value(std::string(it == 0 ? "LE" : "BE"));

        if (!item->setProperty("byte_order", v)) {
            std::cout << "error: " << item->getErrorMessage() << "\n";
            item->clearErrorMessage();
        }
    } else
        std::cout << "error: item_->setProperty(\"type\", u->getValue());\n";

    builder.setData("msb", "MSB");
    builder.setData("bit_depth", "Rank");
    builder.setData("sign", "Sign");
    builder.setData("scale_a1", "A0");
    builder.setData("offset_a2", "A1");


    //std::cout << toString(item->getProperty(""), "") << "\n";
    return item;
}
