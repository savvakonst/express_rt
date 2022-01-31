

#include <clocale>
#include <fstream>
#include <iostream>
#include <regex>

#include "SpecificPParserBuilder.h"
#include "baseio/DefaultBaseIO.h"
#include "common/Extension.h"
#include "common/ExtensionManager.h"
//
#include "EthernetUDP.h"

int initDefaultBaseIO(ExtensionManager *manager) {
    std::setlocale(LC_ALL, "en_US.UTF-8");

    std::string contents;
    {
        std::fstream in("analog.base", std::ios_base::in | std::ios::binary);
        if (!in) return 1;

        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
    }

    DefaultBaseIO base_io;

    auto set = manager->getLastVersionExtensionUintsByType("prm_parser_builder");
    for (auto &i : set) base_io.addPPBM((PDefaultBaseIO_ifs *)i->ptr);

    base_io.parseDocument(manager, contents);
    std::cout << base_io.getErrorMessage();
    return 0;
}

#ifndef DEFAULT_PARAMETERS_LIB_NAME
#    error "DEFAULT_PARAMETERS_NAME undefined"
#endif

static ExtensionUnit g_default_parameters_units[] = {
    {"EthernetUdp", "parameter", "parameter for processing Ethernet UDP",
     (void *)&createParameter<EthernetUdpParameter>, 0x00},
    {"EthernetUdp", "prm_parser_builder", "module for parsing and building EthernetUdp ",
     (void *)(new EthernetUdParserBuilder), 0x00},
    {"Analog", "prm_parser_builder", "module for parsing and building parameter", (void *)(new AnalogParserBuilder),
     0x00},
    {"AnalogVibro", "prm_parser_builder", "module for parsing and building parameter",
     (void *)(new AnalogVibroParserBuilder), 0x00},
    {"AnalogVoltage", "prm_parser_builder", "module for parsing and building parameter",
     (void *)(new AnalogVoltageParserBuilder), 0x00},
    {"DefaultBaseIO", "init",
     "unit with type \"init\" are functions which are running after all modules initialization",
     (void *)initDefaultBaseIO, 0x00},
    {nullptr, nullptr, nullptr, nullptr, 0}};

static ExtensionInfo g_default_parameters_info = {"default parameters", 0x01, g_default_parameters_units};

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, DEFAULT_PARAMETERS_LIB_NAME)(void) {
    return &g_default_parameters_info;
}
