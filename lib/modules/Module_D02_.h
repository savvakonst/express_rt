
#ifndef EXRT_KSDMMODULED02__H
#define EXRT_KSDMMODULED02__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_D02_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_D {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t filter;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_D cnl[24];
    };

#pragma pack()

    Task task_;

   public:
    Module_D02_();

    Module_D02_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Module_D02_();

    std::string getID() const override { return "D02_"; }

    const InfoList* getPropertiesInfoList() override;

    ResValue getProperty(const std::string& prop_path) const override;
    std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override;

    const ErrorInfo_ifs* getErrorInfo(void) const override;
};

#endif
