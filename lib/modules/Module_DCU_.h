
#ifndef EXRT_KSDMMODULEDCU__H
#define EXRT_KSDMMODULEDCU__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_DCU_;

class EthernetDCU_Stream : public ModuleStream_ifs {
   public:
    EthernetDCU_Stream(Module_DCU_ *module);

    ~EthernetDCU_Stream();

    virtual void readFramePeace(ModuleStreamContext_ifs *context, char *ptr, size_t size);

    virtual int getStatistic() {
        // TODO:
        return 0;
    }

    virtual const Module_ifs *getModule() { return module_; }

   protected:
    bool lock_ = true;

    Module_ifs *module_;
    ModuleStream_ifs **sub_modules_;
    size_t number_of_modules_ = 0;
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

    Task task_;

    std::vector<Module_ifs *> modules_;

    size_t size_;
    const std::string module_id_;

    EthernetDCU_Stream *ethernet_stream_ = nullptr;

   public:
    Module_DCU_(const std::string &module_id);

    Module_DCU_(const std::string &name, const void *ptr, size_t size, DeviceBuildingContext_ifs *context);

    ~Module_DCU_();

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

    bool hasTransceiver() const override;

    EthernetSettings getSrcAddress() const override;

    std::string getID() const override { return module_id_; }

    const InfoList *getPropertiesInfoList() override;

    ResValue getProperty(const std::string &prop_path) const override;

    std::string getPropertyAsTxt(const std::string &prop_path) const override;

    bool setProperty(const std::string &prop_path, Value value) override;

    bool setPropertyAsTxt(const std::string &prop_path, const std::string &valie) override;

    const void *getTaskPtr() const override { return (const void *)&task_; }

    size_t getTaskSize() const override;

    ModuleStream_ifs *createModuleStream() override;

    const ErrorInfo_ifs *getErrorInfo(void) const override;
};

#endif
