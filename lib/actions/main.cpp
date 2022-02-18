#include "device/DeviceManager.h"
//
#include "Adapters/Adapters.h"
#include "Ping/ksdconnected.h"
#include "common/ExrtAction_ifs.h"
#include "common/Extension.h"
#include "common/ExtensionManager.h"

//
#ifndef ACTIONS_LIB_NAME
#    error "ACTIONS_LIB_NAME undefined"
#endif

#define REFRESH_ACTION_VER 0x00

class RefreshAction : public ExrtAction_ifs {
   public:
    void init(ExtensionManager *manager) {
        manager_ = manager;
        adapters_ = (Adapters *)manager->getLastVersionExtensionObject("adapters", "adapters");
        device_manager_ = (DeviceManager *)manager->getLastVersionExtensionObject("device_manager", "device_manager");
    }

    bool run() override {
        auto current_adapter = adapters_->getCurrentAdapter();

        std::string error_msg;

        auto connected_devices = devicePing(current_adapter.address_, error_msg);

        if (!error_msg.empty()) {
            std::cout << error_msg;
            return false;
        }

        auto c_device = connected_devices.front();

        const auto &buffer = c_device->getTask();
        std::unique_ptr<Device> device(new Device(buffer.data(), buffer.size(), manager_));
        device->setSource(c_device->getSource());

        device_manager_->addDevice(device.release());

        return true;
    }

    std::string getDescription() override { return ""; }
    std::string getInfo() override { return ""; }

   private:
    ExtensionManager *manager_ = nullptr;
    DeviceManager *device_manager_ = nullptr;
    Adapters *adapters_ = nullptr;
};

static int initActions(ExtensionManager *manager);

static ExtensionUnit g_actions_units[] = {
    {"file/refresh_ethernet_devices", "exrt_action",
     "checks the status of connected ethernet devices and looks for new ones ", (void *)new RefreshAction, REFRESH_ACTION_VER},
    {"adapters", "adapters", "provide available adapters and init socket system", (void *)newAdapters(), 0x00},
    {"actions", "init", "", (void *)&initActions, 0x00},
    {nullptr, nullptr, nullptr, nullptr, 0}};

static ExtensionInfo g_actions_info = {"default actions", 0x01, g_actions_units};

InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, ACTIONS_LIB_NAME)(void) { return &g_actions_info; }

static int initActions(ExtensionManager *manager) {
    auto p_unit = manager->getLastVersionExtensionUnit("exrt_action", "file/refresh_ethernet_devices");
    if ((p_unit == nullptr) || (p_unit->version != REFRESH_ACTION_VER)) {
        DEBUG_CERR("cant init (name: " << p_unit->name << ", type: " << p_unit->type << ", ver.:" << p_unit->version
                                       << ") unit, since there is a newer unit.\n");
    } else
        ((RefreshAction*)p_unit->ptr)->init(manager);


    return 0;
}