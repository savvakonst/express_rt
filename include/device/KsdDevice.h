
#ifndef EXRT_KSDDEVICE_H
#define EXRT_KSDDEVICE_H
#include <vector>

#include "KsdApi/tnmdefs.h"
#include "device/Device_ifs.h"
#include "device/KsdModule.h"

class ExtensionManager;

class KsdDevice : public Device_ifs {
   public:
    KsdDevice(const std::vector<TNMLIB_RECORD_MODULE_INFO> &modules_map, const void *ptr, size_t size,
              ExtensionManager *context)
        : Device_ifs(ptr, size, context), modules_map_(modules_map) {
        auto submodules = modules_.front()->getSubModules();

        auto cnt = 0;
        for (auto &item : submodules)
            if (item.second) ((KSDModule *)item.second)->setStatus(modules_map[cnt++]);
    }

   private:
    const std::vector<TNMLIB_RECORD_MODULE_INFO> modules_map_;
};

#endif  // EXRT_KSDDEVICE_H
