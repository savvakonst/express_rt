#include <iostream>
//

#include "EthernetUDP.h"
#include "SpecificPParserBuilder.h"
#include "common/ExtensionManager.h"

EthernetUdpParameter::EthernetUdpParameter(ExtensionManager* manager) { init(manager); }

EthernetUdpParameter::~EthernetUdpParameter() = default;

PrmBuffer_ifs* EthernetUdpParameter::createBuffer() const {
    // TODO: to implement this function
    return nullptr;
}

std::string EthernetUdpParameter::getType() const { return "ethernet_udp"; }

bool EthernetUdpParameter::isValid() const { return false; }

Parameter_ifs* EthernetUdParserBuilder::parse(ExtensionManager* manager, HierarchicalData_ifs* other,
                                              HierarchicalData_ifs* header, const std::string& path_to_module) const {
    auto item = new EthernetUdpParameter(manager);
    PropBuilder builder = {item, header};
    builder.setData("common/path", Value(path_to_module));

    // TODO: need to  build path

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

    return item;
}
