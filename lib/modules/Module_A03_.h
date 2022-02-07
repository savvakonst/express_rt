
#ifndef EXRT_KSDMMODULEA03__H
#define EXRT_KSDMMODULEA03__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A03_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_03 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t range;
        int32_t current;
        int32_t offset;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_03 cnl[16];
    };

#pragma pack()

    Task task_;

   public:
    Module_A03_();

    Module_A03_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Module_A03_();

    std::string getID() const override { return "A03_"; }

    const DataSchema_ifs* getPropertySchema() override;

    const HierarchicalData_ifs* getProperty(const std::string& prop_path) const override;
    std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value& value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* storeTaskToBuffer() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override;

    const ErrorInfo_ifs* getErrorInfo(void) const override;
};

#endif
