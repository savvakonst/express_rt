
#ifndef EXRT_KSDMMODULEC03__H
#define EXRT_KSDMMODULEC03__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_C03_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_IRIG {
        uint8_t formatNumber;
        uint8_t dataBits;
        uint8_t parity;
        uint8_t syncBits;
        uint64_t syncWord;
        uint16_t frameLength;
        uint8_t typeOfCode;
        uint8_t reserved;
    };

    struct CHANNEL_RS {
        uint32_t frequency;
        uint8_t dataLength;
        uint8_t stopLength;
        uint8_t parity;
        uint8_t typeOfCode;
    };

    struct Task {
        MODULE_HEADER header;
        uint32_t active;
        uint32_t irigMode;
        CHANNEL_IRIG ir[4][4];
        CHANNEL_RS rs[12];
    };

#pragma pack()

    Task task_;

   public:
    Module_C03_();

    Module_C03_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Module_C03_();

    std::string getID() const override { return "C03_"; }

    const DataSchema_ifs* getPropertiesInfoList() override;

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
