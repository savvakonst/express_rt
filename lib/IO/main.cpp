

#include <clocale>
#include <fstream>
#include <iostream>
#include <regex>

#include "base_io/BaseIO.h"
#include "base_io_extensions/EthernetUDP.h"
#include "base_io_extensions/SpecificPParserBuilder.h"
#include "common/Extension.h"
#include "common/ExtensionManager.h"
#include "ksd_io/KsdIO.h"

//

#ifndef COMMON_IO_LIB_NAME
#    error "COMMON_IO_LIB_NAME undefined"
#endif

static int initDefaultBaseIO(ExtensionManager *manager);

static ExtensionUnit g_default_parameters_units[] = {
    {"ksd_io", "io", "instance of class which provides reading \"*.ksd\" files", (void *)new KsdIO, 0x00},
    {"base_io", "io", "instance of class which provides reading \"*.base\" files", (void *)new BaseIO, 0x00},
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

static ExtensionInfo g_default_parameters_info = {"default io interfaces", 0x01, g_default_parameters_units};

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, COMMON_IO_LIB_NAME)(void) { return &g_default_parameters_info; }

static int initDefaultBaseIO(ExtensionManager *manager) {
    // TODO: add guard

    auto e_unit = search(g_default_parameters_units, "io", "base_io");
    if (e_unit == nullptr) {
        DEBUG_CERR("cant find \"base_io\" unit with \"io\" type\n");
        return 1;
    }

    BaseIO *base_io = (BaseIO *)e_unit->ptr;
    auto set = manager->getLastVersionExtensionUnitsByType("prm_parser_builder");
    for (auto &i : set) base_io->addPpbm((PDefaultBaseIO_ifs *)i->ptr);

    return 0;
}