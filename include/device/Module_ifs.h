#ifndef EXO_MODULE_IFS_H
#define EXO_MODULE_IFS_H

#include <list>
#include <string>
#include <vector>

#include "common/Properties_ifs.h"

class ModuleStream_ifs;
class PrmBuffer_ifs;
class ExtensionManager;

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

    const DataSchema_ifs *getPropertySchema() override = 0;

    [[nodiscard]] virtual std::string printProperties(const std::string &indent) const = 0;

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override = 0;

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override = 0;

    bool setProperty(const std::string &prop_path, const Value &value) override = 0;

    bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) override = 0;

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override = 0;

    [[nodiscard]] virtual bool storeTaskToBuffer(void *pointer) const = 0;

    [[nodiscard]] virtual size_t getTaskSize() const = 0;

    [[nodiscard]] virtual std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const = 0;

    virtual ModuleStream_ifs *createModuleStream() = 0;

   protected:
};

template <class T>
Module_ifs *createModule(const void *ptr, size_t size, ExtensionManager *manager) {
    return new T(ptr, size, manager);
}

typedef Module_ifs *(*moduleConstructor_f)(const void *ptr, size_t size, ExtensionManager *manager);

COMMON_API_ std::list<Module_ifs *> getSubmodules(Module_ifs *module, const std::string &glob_pattern);

#endif