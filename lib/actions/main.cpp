
#include "Ping/ksdconnected.h"
#include "common/ExrtAction.h"
#include "common/Extension.h"
#include "common/ExtensionManager.h"

#ifndef ACTIONS_LIB_NAME
#    error "ACTIONS_LIB_NAME undefined"
#endif

class RefreshAction : public ExrtAction {
   public:
    virtual std::string run() {}

    virtual std::string getDescription() {}
    virtual std::string getInfo() {}
};

static ExtensionUnit g_actions_units[] = {
    {"file/refresh_ethernet_devices", "action",
     "checks the status of connected ethernet devices and looks for new ones ", (void *)new RefreshAction, 0x00},
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

static ExtensionInfo g_actions_info = {"default io interfaces", 0x01, g_default_parameters_units};

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, ACTIONS_LIB_NAME)(void) { return &g_default_parameters_info; }

static int initDefaultBaseIO(ExtensionManager *manager) {
    // TODO: add guard

    auto e_unit = search(g_default_parameters_units, "io", "base_io");
    if (e_unit == nullptr) {
        DEBUG_CERR("cant find \"base_io\" unit with \"io\" type\n");
        return 1;
    }

    BaseIO *base_io = (BaseIO *)e_unit->ptr;
    auto set = manager->getLastVersionExtensionUintsByType("prm_parser_builder");
    for (auto &i : set) base_io->addPpbm((PDefaultBaseIO_ifs *)i->ptr);

    return 0;
}