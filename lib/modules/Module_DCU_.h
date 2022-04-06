
#ifndef EXRT_KSDMMODULEDCU__H
#define EXRT_KSDMMODULEDCU__H

#include <cstdint>
#include <map>

#include "device/KsdModule.h"
#include "device/ModuleStream_ifs.h"

class EthernetDCU_Stream;

class Module_DCU_ : public KSDModule {
    friend class EthernetDCU_Stream;

   protected:
#pragma pack(1)

    struct Task {
        MODULE_HEADER header;
    };

#pragma pack()

    Task task_{};

    std::vector<Module_ifs *> modules_;

    size_t number_of_slots_ = 0;
    size_t size_ = 0;
    const std::string module_id_;

   public:
    explicit Module_DCU_(const std::string &module_id);

    Module_DCU_(size_t number_of_slots, const void *ptr, size_t size, ExtensionManager *context);

    ~Module_DCU_() override;

    std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() override {
        std::map<std::string, PrmBuffer_ifs *> ret_map;
        for (auto i : modules_) {
            auto temp = i->getPrmBufferMap();
            for (const auto &j : temp) ret_map[j.first] = j.second;
        }

        return ret_map;
    }

    [[nodiscard]] bool hasTransceiver() const override;

    [[nodiscard]] EthernetSettings getSrcAddress() const override;

    [[nodiscard]] std::string getID() const override { return module_id_; }

    [[nodiscard]] std::list<Module_ifs *> getSubModulesFromPath(const std::string &prop_path) const override;

    [[nodiscard]] const DataSchema_ifs *getPropertySchema() const override;

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override;

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override;

    bool setProperty(const std::string &prop_path, const Value &value) override;

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override;

    [[nodiscard]] bool storeTaskToBuffer(void *pointer) const override {
        // TODO: implement this method
        return false;
    }

    [[nodiscard]] bool isChannelAvailable(const std::string &prop_path) const override;

    [[nodiscard]] size_t getTaskSize() const override;

    ModuleStream_ifs *createModuleStream() override;

    [[nodiscard]] std::vector<std::pair<std::string, Module_ifs *>> getSubModules() const override;

    [[nodiscard]] const ErrorInfo_ifs *getErrorInfo() const override;
};

template <size_t SLOTS_NUMBER>
Module_ifs *createCHXXModule(const void *ptr, size_t size, ExtensionManager *manager) {
    static_assert(SLOTS_NUMBER < 100);
    return new Module_DCU_(SLOTS_NUMBER, ptr, size, manager);
}

#endif
