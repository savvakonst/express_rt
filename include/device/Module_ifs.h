#ifndef EXO_MODULE_IFS_H
#define EXO_MODULE_IFS_H

#include <list>
#include <string>
#include <vector>

#include "common/Properties_ifs.h"

class ModuleStream_ifs;
class PrmBuffer_ifs;
class ExtensionManager;

inline std::string stringId(uint32_t id) {
    char char_id[5] = {0, 0, 0, 0, 0};
    *((uint32_t *)char_id) = id;
    return char_id;
}

class COMMON_API_ Module_ifs : public Properties_ifs {
   public:
    ~Module_ifs() override = default;

    [[nodiscard]] virtual bool hasTransceiver() const = 0;

    [[nodiscard]] virtual EthernetSettings getSrcAddress() const = 0;

    [[nodiscard]] virtual bool isAvailable() const = 0;

    [[nodiscard]] virtual std::string getID() const = 0;

    [[nodiscard]] virtual std::string getModulePath(bool full_path = true) const = 0;

    [[nodiscard]] virtual Module_ifs *getParentModule() const = 0;

    virtual bool setParentModule(Module_ifs *) = 0;

    [[nodiscard]] virtual std::list<Module_ifs *> getSubModulesFromPath(const std::string &module_path) const = 0;

    virtual std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() = 0;

    [[nodiscard]] const DataSchema_ifs *getPropertySchema() const override = 0;

    [[nodiscard]] virtual std::string printProperties(const std::string &indent) const = 0;

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override = 0;

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override = 0;

    bool setProperty(const std::string &prop_path, const Value &value) override = 0;

    bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) override = 0;

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override = 0;

    bool removeProperty(const std::string &prop_path) override = 0;  // { return false;}

    [[nodiscard]] virtual bool isChannelAvailable(const std::string &path) const = 0;

    [[nodiscard]] virtual Value getChannelProperty(const std::string &channel, const std::string &type) const = 0;

    [[nodiscard]] virtual bool storeTaskToBuffer(void *pointer) const = 0;

    [[nodiscard]] virtual size_t getTaskSize() const = 0;

    [[nodiscard]] virtual std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const = 0;

    virtual ModuleStream_ifs *createModuleStream() = 0;

    virtual ModuleStream_ifs *getModuleStream() = 0;

    virtual bool deattachModuleStream() = 0;

   protected:
};

template <class T>
Module_ifs *createModule(const void *ptr, size_t size, ExtensionManager *manager) {
    return new T(ptr, size, manager);
}

typedef Module_ifs *(*moduleConstructor_f)(const void *ptr, size_t size, ExtensionManager *manager);

COMMON_API_ std::list<Module_ifs *> getSubmodules(Module_ifs *module, const std::string &glob_pattern);

#endif