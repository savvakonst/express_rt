
#ifndef EXRT_KSDMMODULEID02_H
#define EXRT_KSDMMODULEID02_H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_ID02 : public KSDModule {
   protected:
#pragma pack(1)

    struct IMIT_CHANNEL_D {
        uint8_t state;
        uint8_t reservedA;
        uint16_t onCode;
        uint16_t offCode;
        uint16_t width;
        uint16_t frequency;
        uint8_t record;
        uint8_t reservedB;
    };

    struct Task {
        MODULE_HEADER header;
        IMIT_CHANNEL_D out[2];
    };

#pragma pack()

    Task task_;

   public:
    Module_ID02();

    Module_ID02(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Module_ID02();

    std::string getID() const override { return "ID02"; }

    const DataSchema_ifs* getPropertiesInfoList() override;

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
