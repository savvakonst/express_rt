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
#include "common/StringProcessingTools.h"
#include "device/Module_ifs.h"
//

#ifndef READER_LIB_NAME
#    error "READER_LIB_NAME undefined"
#endif

static int initDefaultBaseIO(ExtensionManager *manager);

template <class T>
PrmBuffer_ifs *createDynamicSyncPrmBuffer(const Module_ifs *module, Parameter_ifs *parameter,
                                          ExtensionManager *manager) {
    // TODO: create function getChannel
    auto full_path = parameter->getProperty("common/path")->getValue().asString();
    auto path = lastCharPos(full_path, '/');
    auto fr = module->getChannelProperty(path.second, "frequency").value_.u8;

    return new PseudoSyncPrmBuffer(parameter, fr, nullptr);
}

static ExtensionUnit g_default_parameters_units[] = {
    {"ethernet_udp", "dynamic_prm_buffer", "parameter for processing Ethernet UDP",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},
    {"analog", "dynamic_prm_buffer", "parameter for processing simple analog voltage signals",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},
    {"acceleration", "dynamic_prm_buffer", "parameter for processing acceleration",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},
    {"velocity", "dynamic_prm_buffer", "parameter for processing velocity",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},
    {"displacement", "dynamic_prm_buffer", "parameter for processing displacement",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},
    {"thermocouple_emf", "dynamic_prm_buffer", "parameter for processing thermocouple_emf",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},
    {"thermocouple_temperature", "dynamic_prm_buffer", "parameter for processing thermocouple_temperature",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},
    {"thermistor_resistance", "dynamic_prm_buffer", "parameter for processing thermistor_resistance",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},
    {"thermistor_temperature", "dynamic_prm_buffer", "parameter for processing thermistor_temperature",
     (void *)&createDynamicSyncPrmBuffer<PseudoSyncPrmBuffer>, 0x00},

    {nullptr, nullptr, nullptr, nullptr, 0}};

static ExtensionInfo g_default_parameters_info = {"PrmBuffers", 0x01, g_default_parameters_units};

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, READER_LIB_NAME)(void) { return &g_default_parameters_info; }
