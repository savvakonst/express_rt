#ifndef EXO_DEVICE_H
#define EXO_DEVICE_H
#include <string>

#include "common/BaseClass_ifs.h"
#include "common/Port.h"

class Module_ifs;

class COMMON_API_ Device : public BaseClass_ifs {
   private:
   public:
    Device(/* args */);

    ~Device();

    exo_container<const Module_ifs*> getAllModules();

    const Module_ifs* getModuleFromPath(std::string name);

    exo_container<const Module_ifs*> getLineFromPath(std::string path);

    status checkValExistence(std::string path);

    status isTranciverEnabled();

    std::string getSrcAddress();
};

#endif