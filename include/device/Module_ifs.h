#ifndef EXO_MODULE_IFS_H
#define EXO_MODULE_IFS_H

#include <string>

#include "common/BaseClass_ifs.h"
#include "common/CustomTypes.h"
#include "common/DataSchema_ifs.h"

class ModuleStream_ifs;
class PrmBuffer_ifs;
class DeviceBuildingContext_ifs;

class COMMON_API_ Module_ifs : public BaseClass_ifs {
   private:
    /* data */
   public:
    virtual ~Module_ifs(){};

    virtual bool hasTransceiver() const = 0;

    virtual EthernetSettings getSrcAddress() const = 0;

    virtual bool isAvailable() const = 0;

    [[nodiscard]] virtual std::string getID() const = 0;

    virtual std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() {
        return std::map<std::string, PrmBuffer_ifs *>();
    }

    virtual const DataSchema_ifs *getPropertiesInfoList() = 0;

    [[nodiscard]] virtual std::string printProperties(const std::string &indent = "") const = 0;

    [[nodiscard]] virtual const HierarchicalData_ifs *getProperty(const std::string &prop_path) const = 0;

    [[nodiscard]] virtual std::string getPropertyAsTxt(const std::string &prop_path) const = 0;

    virtual bool setProperty(const std::string &prop_path, Value value) = 0;

    virtual bool setPropertyAsTxt(const std::string &prop_path, const std::string &valie) = 0;

    [[nodiscard]] virtual const void *getTaskPtr() const = 0;

    [[nodiscard]] virtual size_t getTaskSize() const = 0;

    virtual Module_ifs *getSubModule() { return nullptr; }

    virtual ModuleStream_ifs *createModuleStream() = 0;
};

template <class T>
Module_ifs *createModule(const void *ptr, size_t size, DeviceBuildingContext_ifs *context) {
    return new T(ptr, size, context);
}

typedef Module_ifs *(*moduleConstructor_f)(const void *ptr, size_t size, DeviceBuildingContext_ifs *context);

#endif