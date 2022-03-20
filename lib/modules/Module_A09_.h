
#ifndef EXRT_KSDMMODULEA09__H
#define EXRT_KSDMMODULEA09__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A09_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_09 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t range;
        int32_t reference;
        int32_t sinRange;
        int32_t sinFreq;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_09 cnl[6];
        uint8_t reserved[40];
    };

#pragma pack()

    Task task_;

   public:
    Module_A09_();

    Module_A09_(const void* ptr, size_t size, ExtensionManager* manager);

    ~Module_A09_();

    [[nodiscard]] std::string getID() const override { return "A09_"; }

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
