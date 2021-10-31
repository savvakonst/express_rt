#ifndef EXO_DEVICE_H
#define EXO_DEVICE_H
#include <string>

#include "common/BaseClass_ifs.h"
#include "common/Port.h"
#include "device/Module_ifs.h"

class Module_ifs;

class DeviceBuildingContext_ifs {
   public:
    DeviceBuildingContext_ifs() {}
    virtual moduleConstructor_f getConstructor() = 0;

    virtual Module_ifs* createMdule(const std::string& module_id, const void* ptr, size_t size,
                                    DeviceBuildingContext_ifs* context) {
        return context->getConstructor()(ptr, size, context);
    }
};

class COMMON_API_ Device : public BaseClass_ifs {
   private:
    std::vector<Module_ifs*> modules_;
    //
   public:
    Device(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Device();

    exo_container<const Module_ifs*> getAllModules();

    const Module_ifs* getModuleFromPath(std::string name);

    exo_container<const Module_ifs*> getLineFromPath(std::string path);

    status checkValExistence(std::string path);

    status isTranciverEnabled();

    std::string getSrcAddress();
};

#endif