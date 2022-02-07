
#ifndef EXRT_KSDMMODULED01__H
#define EXRT_KSDMMODULED01__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_D01_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_COUNTER {
        uint8_t flags;
        int32_t voltageP;
        int32_t voltageN;
        int32_t filter;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_COUNTER cnl[16];
    };

#pragma pack()

    Task task_;

   public:
    Module_D01_();

    Module_D01_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Module_D01_();

    std::string getID() const override { return "D01_"; }

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
