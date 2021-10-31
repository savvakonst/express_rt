
#ifndef EXRT_KSDMMODULEA01__H
#define EXRT_KSDMMODULEA01__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A01_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_01 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t rangeH;
        int32_t rangeL;
        uint32_t ureserved;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_01 cnl[32];
    };

#pragma pack()

    Task task_;

   public:
    Module_A01_();

    Module_A01_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Module_A01_() {}

    std::string getID() const override { return "A01_"; }

    // const InfoList& getPropertiesInfoList() override;

    // const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    // const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    // bool setProperty(const std::string& prop_path, Value value) override;
    // bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override {
        error_mesadge_ = "The createModuleStream function is not realised yet";
        return nullptr;
    };

    const ErrorInfo_ifs* getErrorInfo(void) const override { return nullptr; }
};

#endif
