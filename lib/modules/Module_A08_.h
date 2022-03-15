
#ifndef EXRT_KSDMMODULEA08__H
#define EXRT_KSDMMODULEA08__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A08_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_08 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t range;
        uint32_t ureserved[2];
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_08 cnl[9];
        uint8_t reserved[112];
    };

#pragma pack()

    Task task_;

   public:
    Module_A08_();

    Module_A08_(const void* ptr, size_t size, ExtensionManager* context);

    ~Module_A08_() override;

    [[nodiscard]] std::string getID() const override { return "A08_"; }

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

    [[nodiscard]] const ErrorInfo_ifs* getErrorInfo() const override;
};

#endif
