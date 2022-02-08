

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

#ifndef DEFAULT_PARAMETERS_LIB_NAME
#    error "DEFAULT_PARAMETERS_NAME undefined"
#endif

static int initDefaultBaseIO(ExtensionManager *manager);

static ExtensionUnit g_default_parameters_units[] = {
    {"base_io", "io", "instance of class which provides reading \"*.base\" files", (void *)new DefaultBaseIO, 0x00},
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

static int initDefaultBaseIO(ExtensionManager *manager) {
    // TODO: add guard

    auto e_unit = search(g_default_parameters_units, "io", "base_io");
    if (e_unit == nullptr) {
        DEBUG_CERR("cant find \"base_io\" unit with \"io\" type\n");
        return 1;
    }

    DefaultBaseIO *base_io = (DefaultBaseIO *)e_unit->ptr;
    auto set = manager->getLastVersionExtensionUintsByType("prm_parser_builder");
    for (auto &i : set) base_io->addPpbm((PDefaultBaseIO_ifs *)i->ptr);

    return 0;
}