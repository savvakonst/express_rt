
#ifndef EXRT_KSDMMODULEC06__H
#define EXRT_KSDMMODULEC06__H

#include <cstdint>
#include <map>

#include "device/KsdModule.h"
#include "device/ModuleStream_ifs.h"

class Module_C06_ : public KSDModule {
   protected:
#pragma pack(1)

    struct Task {
        MODULE_HEADER header;
        uint8_t inputs;
        uint8_t reserved[31];
    };

#pragma pack()

    Task task_;

   public:
    Module_C06_();

    Module_C06_(const void* ptr, size_t size, ExtensionManager* manager);

    ~Module_C06_();

    [[nodiscard]] std::string getID() const override { return "C06_"; }

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
