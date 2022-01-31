
#ifndef EXRT_KSDMMODULEA05__H
#define EXRT_KSDMMODULEA05__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A05_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_05 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t range;
        uint32_t ureserved[2];
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_05 cnl[9];
        uint8_t reserved[112];
    };

#pragma pack()

    Task task_;

   public:
    Module_A05_();

    Module_A05_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Module_A05_();

    std::string getID() const override { return "A05_"; }

    const DataSchema_ifs* getPropertySchema() override;

    const HierarchicalData_ifs* getProperty(const std::string& prop_path) const override;
    std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override;

    const ErrorInfo_ifs* getErrorInfo(void) const override;
};

#endif
