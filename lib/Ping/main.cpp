

#include <stdio.h>

#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "Device/Device.h"
#include "Receiver/Receiver.h"
#include "common/extension.h"
#include "ksdconnected.h"

bool initWSA() {
    WSADATA wsa;
    int32_t status = ::WSAStartup(MAKEWORD(2, 2), &wsa);
    if (status) return false;
    return true;
}

class DeviceBuildingContext : public DeviceBuildingContext_ifs {
   public:
    ~DeviceBuildingContext() {}

    bool addUint(ExtensionUnit *uint) {
        if (std::string(uint->type) == "module") map_[uint->name] = (moduleConstructor_f)uint->ptr;
        return true;
    }

    moduleConstructor_f getConstructor(const std::string &module_id) override {
        auto e = map_.find(module_id);
        if (e == map_.end()) {
            return nullptr;
        } else {
            return e->second;
        }
    }

    Module_ifs *createModule(const std::string &module_id, const void *ptr, size_t size,
                             DeviceBuildingContext_ifs *context) override {
        moduleConstructor_f constructor = context->getConstructor(module_id);
        if (constructor) return constructor(ptr, size, context);
        return nullptr;
    }

   private:
    std::map<std::string, moduleConstructor_f> map_;
};

InitExtension(ExtensionInfo *) initModules(void);
/*
int main(int argc, char *argv[]) {
    initWSA();

    ExtensionInfo *extension_info = initModules();
    auto units = extension_info->units;

    DeviceBuildingContext building_context;

    while (units->name) {
        building_context.addUint(units++);
    }

    std::string error_msg;
    auto devices = devicePing(error_msg);
    auto v = devices.front()->getTask();

    Device device = Device(v.data(), v.size(), &building_context);

    if (device.hasError()) std::cout << "error: " << device.getErrorMessage() + "\n";
    if (device.hasTransceiver()) {
        ModuleStream_ifs *m_stream = device.getTopModule()->createModuleStream();
        Receiver receiver = Receiver(m_stream, device.getSrcAddress());
        receiver.start();
    }
    return 0;
}
 */