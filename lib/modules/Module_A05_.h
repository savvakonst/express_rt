
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

    Module_A05_(const void* ptr, size_t size, ExtensionManager* context);

    ~Module_A05_();

    [[nodiscard]] std::string getID() const override { return "A05_"; }

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
