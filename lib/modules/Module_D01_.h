
#ifndef EXRT_KSDMMODULED01__H
#define EXRT_KSDMMODULED01__H

#include <cstdint>
#include <map>

#include "device/KsdModule.h"
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

    Module_D01_(const void* ptr, size_t size, ExtensionManager* manager);

    ~Module_D01_();

    [[nodiscard]] std::string getID() const override { return "D01_"; }

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
