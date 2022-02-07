
#ifndef EXRT_KSDMMODULEA11__H
#define EXRT_KSDMMODULEA11__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A11_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_11 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t range;
        uint32_t reference;
        uint32_t ureserved;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_11 cnl[3];
        uint8_t reserved[16];
    };

#pragma pack()

    Task task_;

   public:
    Module_A11_();

    Module_A11_(const void* ptr, size_t size, ExtensionManager* context);

    ~Module_A11_();

    std::string getID() const override { return "A11_"; }

    const DataSchema_ifs* getPropertySchema() override;

    const HierarchicalData_ifs* getProperty(const std::string& prop_path) const override;
    std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value& value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    bool storeTaskToBuffer(void* pointer) const override {
        memcpy(pointer, (void*)&task_, sizeof(Task));
        return true;
    }
    size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override;

    const ErrorInfo_ifs* getErrorInfo(void) const override;
};

#endif
