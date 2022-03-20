
#ifndef EXRT_KSDMMODULEC04__H
#define EXRT_KSDMMODULEC04__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_C04_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_CAN {
        uint32_t frequency;
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
        CHANNEL_CAN can[6];
        CHANNEL_RS rs[12];
    };

#pragma pack()

    Task task_;

   public:
    Module_C04_();

    Module_C04_(const void* ptr, size_t size, ExtensionManager* manager);

    ~Module_C04_();

    [[nodiscard]] std::string getID() const override { return "C04_"; }

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
