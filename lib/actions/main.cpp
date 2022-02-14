
#include "Adapters/Adapters.h"
#include "Ping/ksdconnected.h"
#include "common/ExrtAction.h"
#include "common/Extension.h"
#include "common/ExtensionManager.h"
#ifndef ACTIONS_LIB_NAME
#    error "ACTIONS_LIB_NAME undefined"
#endif

class RefreshAction : public ExrtAction {
   public:
    bool run() override {
        devicePing();
        return true;
    }

    std::string getDescription() override {}
    std::string getInfo() override {}
};

static ExtensionUnit g_actions_units[] = {
    {"file/refresh_ethernet_devices", "action",
     "checks the status of connected ethernet devices and looks for new ones ", (void *)new RefreshAction, 0x00},
    {"file/refresh_ethernet_devices", "adapter",
     "checks the status of connected ethernet devices and looks for new ones ", (void *)new RefreshAction, 0x00},
    //{"socket", "init", "init wsa if windows", (void *)initSocket, 0x00},
    {nullptr, nullptr, nullptr, nullptr, 0}};

static ExtensionInfo g_actions_info = {"default actions", 0x01, g_actions_units};

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, ACTIONS_LIB_NAME)(void) {
    initSocket();
    return &g_actions_info;
}

static int initSocket(ExtensionManager *manager) { return 0; }