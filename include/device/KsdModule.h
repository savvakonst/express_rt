#ifndef EXRT_KSDMODULE_H
#define EXRT_KSDMODULE_H

#include <cstdint>
#include <map>
#include <utility>

#include "TaskMapper.h"
#include "device/ModuleStream_ifs.h"
#include "device/Module_ifs.h"

/*
 *
 *
 *
 */

class ExtensionManager;

class KSDModule : public Module_ifs {
   protected:
    explicit KSDModule(const TaskMapper &field_map) : field_map_(field_map) {}

#pragma pack(1)
    struct MODULE_HEADER {
        uint32_t size;
        uint32_t id;
        uint8_t sub;
        uint8_t slot;
        uint16_t version;
        uint16_t check_sum;
        uint16_t flag;
        uint16_t dimension;
        uint16_t sync_mode;
        uint8_t reserved[12];
    };
#pragma pack()

    static const TaskMapper header_map_;

    TaskMapper field_map_;

    Module_ifs *parent_module_ = nullptr;

    ModuleStream_ifs *ethernet_stream_ = nullptr;

    [[nodiscard]] const TaskMapper *getBranch(const std::string &prop_path) const;

   public:
    [[nodiscard]] bool hasTransceiver() const override { return false; }

    [[nodiscard]] EthernetSettings getSrcAddress() const override { return {}; }

    [[nodiscard]] bool isAvailable() const override { return true; }

    [[nodiscard]] std::string getModulePath(bool full_path) const override;

    [[nodiscard]] Module_ifs *getParentModule() const override { return parent_module_; }

    bool setParentModule(Module_ifs *parent_module) override {
        if (parent_module_) return true;
        parent_module_ = parent_module;
        return true;
    }

    [[nodiscard]] std::list<Module_ifs *> getSubModulesFromPath(const std::string &prop_path) const override {
        return {};
    }

    std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() override { return {}; }

    [[nodiscard]] const DataSchema_ifs *getPropertySchema() const override {
        // error_message_ = "The getPropertySchema function is not realised yet";
        return nullptr;
    }

    [[nodiscard]] std::string printProperties(const std::string &indent) const override;

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override;

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override;

    bool setProperty(const std::string &prop_path, const Value &value) override;

    bool setProperty(const std::string &prop_path, const HierarchicalData_ifs *hierarchical_data) override {
        return false;
    }

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override;

    bool removeProperty(const std::string &prop_path) override { return false; }

    [[nodiscard]] bool isChannelAvailable(const std::string &prop_path) const override;

    [[nodiscard]] Value getChannelProperty(const std::string &prop_path, const std::string &type) const override {
        return {};
    }

    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const override { return {}; }

    ModuleStream_ifs *getModuleStream() override { return ethernet_stream_; }

    bool removeModuleStream() override {
        if (ethernet_stream_) {
            delete ethernet_stream_;
            ethernet_stream_ = nullptr;
            return true;
        }
        return false;
    }

   protected:
};

#endif
