

#include "EthernetParmaeter.h"

/*
 * info defenition
 */

std::vector<FieldInfo*> g_ethernet_parameter_fields; /*= {
    new FieldInfo{DataType::string_val, u8"others/destenation/ip", u8"ip", u8"ip help"},
    new FieldInfo{DataType::string_val, u8"others/destenation/port", u8"порт", u8"порт help"},
    new FieldInfo{DataType::string_val, u8"others/source/ip", u8"ip", u8"category help"},
    new FieldInfo{DataType::string_val, u8"others/source/port", u8"порт", u8"system help"},
    new FieldInfo{DataType::uint_val, u8"others/offset", u8"смещение", u8"department help"},
    newStringEnumFieldInfo(u8"others/type", {"i8", "i16", "i32", "i64", "f32", "f64"}, u8"тип\\/размер", u8""),
    newStringEnumFieldInfo(u8"others/byte_ordr", {"BE", "LE"}, u8"порядок байт", u8""),
    newUintEnumFieldInfo(u8"others/msb", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}, "старший разряд"),
    new FieldInfo{DataType::uint_val, u8"others/bit_width", u8"старший разряд", u8""}};*/

InfoList EthernetParameter::info_list_ = InfoList({&g_common_parameter_fields, &g_ethernet_parameter_fields});

/*
 * class members
 */

EthernetParameter::EthernetParameter(const std::string& name) : Parameter_ifs(name) {
    for (auto i : info_list_) {
        // fields_map[i->name_] =  // SelfValue(i);
    }
}

EthernetParameter* newEthernetParameter(const std::string& name) { return new EthernetParameter(name); }

PrmBuffer_ifs* EthernetParameter::createBuffer() const { return nullptr; }

const std::string EthernetParameter::getType() const { return "ethernet"; }

const ResValue_ifs* EthernetParameter::getProperty(const std::string& prop_path) const {}

// const std::string EthernetParameter::getPropertyAsTxt(const std::string& prop_path) const;

// bool EthernetParameter::setProperty(const std::string& prop_path, const InValue value);

// bool EthernetParameter::setPropertyAsTxt(const std::string& prop_path, const std::string& valie);

const InfoList& EthernetParameter::getPropertiesInfoList() { return info_list_; };

// const bool EthernetParameter::isValid() const;

const ErrorInfo_ifs* EthernetParameter::getErrorInfo() const { return nullptr; }

/*
 * interface to linking this extension
 */

static struct ExtensionUint g_ext_uints[] = {  //
    {"ethernet", "field_info", "", (void*)(&g_ethernet_parameter_fields), 0},
    {"ethernet", "field_info", "", (void*)(&g_ethernet_parameter_fields), 0},
    {nullptr, nullptr, nullptr, nullptr, 0}};

static struct ExtensionInfo g_ext_info = {"ethernet extensoon", 0, g_ext_uints};

InitExtension(ExtensionInfo*) initExtEthernet() { return &g_ext_info; }
