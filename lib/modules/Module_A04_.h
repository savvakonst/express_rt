
#ifndef EXRT_KSDMMODULEA04__H
#define EXRT_KSDMMODULEA04__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A04_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_04 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t range;
        uint32_t ureserved[2];
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_04 cnl[12];
        uint8_t reserved[64];
    };

#pragma pack()

    Task task_;

   public:
    Module_A04_();

    Module_A04_(const void* ptr, size_t size, ExtensionManager* context);

    ~Module_A04_() override;

    [[nodiscard]] std::string getID() const override { return "A04_"; }

    [[nodiscard]] const DataSchema_ifs* getPropertySchema() const override;

    [[nodiscard]] const HierarchicalData_ifs* getProperty(const std::string& prop_path) const override;
    [[nodiscard]] std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value& value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    [[nodiscard]] bool isChannelAvailable(const std::string& prop_path) const override;

    [[nodiscard]] Value getChannelProperty(const std::string& channel, const std::string& type) const override;

    bool storeTaskToBuffer(void* pointer) const override {
        memcpy(pointer, (void*)&task_, sizeof(Task));
        return true;
    }
    [[nodiscard]] size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override;

    [[nodiscard]] const ErrorInfo_ifs* getErrorInfo() const override;

    [[nodiscard]] const Task& getTask() const { return task_; }
};

#endif
