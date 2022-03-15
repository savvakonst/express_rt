
#ifndef EXRT_KSDMMODULEDCU__H
#define EXRT_KSDMMODULEDCU__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_DCU_;

class EthernetDCU_Stream : public ModuleStream_ifs {
   public:
    explicit EthernetDCU_Stream(Module_DCU_ *module);

    ~EthernetDCU_Stream() override;

    void readFramePeace(ModuleStreamContext_ifs *context, char *ptr, size_t size) override;

    int getStatistic() override {
        // TODO:
        return 0;
    }

    const Module_ifs *getModule() override { return module_; }

   protected:
    bool lock_ = true;

    Module_ifs *module_;
    ModuleStream_ifs **sub_modules_;
    size_t number_of_slots_ = 0;
    size_t data_offset_ = 0;
};

class Module_DCU_ : public KSDModule {
    friend EthernetDCU_Stream;

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

    EthernetDCU_Stream *ethernet_stream_ = nullptr;

   public:
    explicit Module_DCU_(const std::string &module_id);

    Module_DCU_(size_t number_of_slots, const void *ptr, size_t size, ExtensionManager *context);

    ~Module_DCU_() override;

    std::map<std::string, PrmBuffer_ifs *> getPrmBufferMap() override {
        std::map<std::string, PrmBuffer_ifs *> ret_map;
        for (auto i : modules_) {
            auto temp = i->getPrmBufferMap();
            for (auto i : temp) {
                ret_map[i.first] = i.second;
            }
        }

        return ret_map;
    }

    [[nodiscard]] bool hasTransceiver() const override;

    [[nodiscard]] EthernetSettings getSrcAddress() const override;

    [[nodiscard]] std::string getID() const override { return module_id_; }

    [[nodiscard]] std::list<Module_ifs *> getSubModulesFromPath(const std::string &prop_path) const override;

    const DataSchema_ifs *getPropertySchema() const override;

    [[nodiscard]] const HierarchicalData_ifs *getProperty(const std::string &prop_path) const override;

    [[nodiscard]] std::string getPropertyAsTxt(const std::string &prop_path) const override;

    bool setProperty(const std::string &prop_path, const Value &value) override;

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &value) override;

    [[nodiscard]] bool storeTaskToBuffer(void *pointer) const override {
        // TODO: implement this method
        return false;
    }

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
