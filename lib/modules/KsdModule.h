#ifndef EXRT_KSDMODULE_H
#define EXRT_KSDMODULE_H

#include <cstdint>
#include <map>
#include <utility>

#include "device/ModuleStream_ifs.h"
#include "device/Module_ifs.h"

class TaskMapper : public HierarchicalData_ifs {
   public:
    enum StructType
    {
        value,
        array,
        map,
    };
    const StructType struct_type_;

    TaskMapper(const TaskMapper &task_mapper);

    TaskMapper(DataType type);

    TaskMapper(size_t len, const TaskMapper &value);

    typedef std::pair<std::string, TaskMapper> tuple_t;

    explicit TaskMapper(std::vector<tuple_t> vecmap);

    ~TaskMapper() override;

    [[nodiscard]] bool isArray() const override { return struct_type_ == StructType::array; };

    [[nodiscard]] bool isMap() const override { return struct_type_ == StructType::map; };

    [[nodiscard]] bool isValue() const override { return struct_type_ == StructType::value; };

    [[nodiscard]] Value getValue() const override {  // necessarry to add guard
        return {ptr_, type_};
    };

    [[nodiscard]] std::vector<HierarchicalData_ifs *> getArray() const override;

    [[nodiscard]] getMapReturn_t getMap() const override;

    [[nodiscard]] HierarchicalData_ifs *getArrayUnit(size_t id) const override;

    [[nodiscard]] HierarchicalData_ifs *getMapUnit(const std::string &id) const override;

    [[maybe_unused]] bool setValue(const Value &data);

    [[maybe_unused]] bool setValue(const std::string &data);

    void setReferencePtr(void *ptr);

   private:
    size_t size_ = 0;
    DataType type_ = DataType::none_v;
    void *ptr_ = nullptr;
    std::vector<TaskMapper> vector_;
    std::vector<tuple_t> vecmap_;
    std::map<std::string, HierarchicalData_ifs *> map_;
};

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

    bool isChannelAvailable(const std::string &prop_path) const override;

    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const override { return {}; }

   protected:
};

#endif
