
#ifndef EXRT_KSDMMODULEID02_H
#define EXRT_KSDMMODULEID02_H

#include <cstdint>
#include <map>

#include "device/KsdModule.h"
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

    Module_ID02(const void* ptr, size_t size, ExtensionManager* manager);

    ~Module_ID02();

    [[nodiscard]] std::string getID() const override { return "ID02"; }

    [[nodiscard]] const DataSchema_ifs* getPropertySchema() const override;

    [[nodiscard]] const HierarchicalData_ifs* getProperty(const std::string& prop_path) const override;
    [[nodiscard]] std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value& value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    bool storeTaskToBuffer(void* pointer) const override {
        memcpy(pointer, (void*)&task_, sizeof(Task));
        return true;
    }
    [[nodiscard]] size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override;

    [[nodiscard]] const ErrorInfo_ifs* getErrorInfo(void) const override;
};

#endif
