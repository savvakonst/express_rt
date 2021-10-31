
#ifndef EXRT_KSDMMODULEA06__H
#define EXRT_KSDMMODULEA06__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A06_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_06 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t range;
        int32_t current;
        int32_t offset;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_06 cnl[10];
        uint8_t reserved[96];
    };

#pragma pack()

    Task task_;

   public:
    Module_A06_()
        : KSDModule(  //
              TaskMapper({{"header", header_map_},
                          {"cnl",
                           {10, TaskMapper({{"frequency", u8},
                                            {"reserved", {2, u8}},
                                            {"flags", u8},
                                            {"range", i32},
                                            {"current", i32},
                                            {"offset", i32}})}},
                          {"reserved", {96, u8}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_A06_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_A06_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_A06_() {}

    std::string getID() const override { return "A06_"; }

    // const InfoList& getPropertiesInfoList() override;

    // const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    // const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    // bool setProperty(const std::string& prop_path, Value value) override;
    // bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override {
        error_mesadge_ = "The createModuleStream function is not realised yet";
        return nullptr;
    };

    const ErrorInfo_ifs* getErrorInfo(void) const override { return nullptr; }
};

#endif