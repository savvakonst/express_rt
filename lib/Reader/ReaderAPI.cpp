//
// Created by SVK on 30.03.2022.
//
#include <clocale>
#include <fstream>
#include <iostream>
#include <regex>

#include "IntervalBuffer.h"
#include "PseudoSyncPrmBuffer.h"
#include "common/Extension.h"
#include "common/ExtensionManager.h"

//

#ifndef READER_LIB_NAME
#    error "READER_LIB_NAME undefined"
#endif

static int initDefaultBaseIO(ExtensionManager *manager);

template<class T>
PrmBuffer_ifs* createDynamicPrmBuffer(Parameter_ifs *parameter,ExtensionManager *manager){
    return nullptr;
}

static ExtensionUnit g_default_parameters_units[] = {
    {"ethernet_udp", "dynamic_prm_buffer", "parameter for processing Ethernet UDP",
     (void *)&createParameter<EthernetUdpParameter>, 0x00},
    {"analog", "dynamic_prm_buffer", "parameter for processing simple analog voltage signals",
     (void *)&createParameter<AnalogParameter>, 0x00},
    {"acceleration", "dynamic_prm_buffer", "parameter for processing acceleration",
     (void *)&createParameter<AccelerationParameter>, 0x00},
    {"velocity", "dynamic_prm_buffer", "parameter for processing velocity", (void *)&createParameter<VelocityParameter>, 0x00},
    {"displacement", "dynamic_prm_buffer", "parameter for processing displacement",
     (void *)&createParameter<DisplacementParameter>, 0x00},
    {"thermocouple_emf", "dynamic_prm_buffer", "parameter for processing thermocouple_emf",
     (void *)&createParameter<ThermocoupleEmfParameter>, 0x00},
    {"thermocouple_temperature", "dynamic_prm_buffer", "parameter for processing thermocouple_temperature",
     (void *)&createParameter<ThermocoupleTemperatureParameter>, 0x00},
    {"thermistor_resistance", "dynamic_prm_buffer", "parameter for processing thermistor_resistance",
     (void *)&createParameter<ThermistorResistanceParameter>, 0x00},
    {"thermistor_temperature", "dynamic_prm_buffer", "parameter for processing thermistor_temperature",
     (void *)&createParameter<ThermistorTemperatureParameter>, 0x00},

    {nullptr, nullptr, nullptr, nullptr, 0}};

static ExtensionInfo g_default_parameters_info = {"default io interfaces", 0x01, g_default_parameters_units};

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, READER_LIB_NAME)(void) { return &g_default_parameters_info; }

static int initDefaultBaseIO(ExtensionManager *manager) {
    // TODO: add guard

    auto e_unit = search(g_default_parameters_units, "io", "base_io");
    if (e_unit == nullptr) {
        DEBUG_CERR("cant find \"base_io\" unit with \"io\" type\n");
        return 1;
    }

    auto *base_io = (BaseIO *)e_unit->ptr;
    auto set = manager->getLastVersionExtensionUnitsByType("prm_parser_builder");
    for (auto &i : set) base_io->addPpbm((PDefaultBaseIO_ifs *)i->ptr);

    return 0;
}