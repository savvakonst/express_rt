
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
    Module_A09_()
        : KSDModule(  //
              TaskMapper({{"header", header_map_},
                          {"cnl",
                           {6, TaskMapper({{"frequency", u8},
                                           {"reserved", {2, u8}},
                                           {"flags", u8},
                                           {"range", i32},
                                           {"reference", i32},
                                           {"sinRange", i32},
                                           {"sinFreq", i32}})}},
                          {"reserved", {40, u8}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_A09_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_A09_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_A09_() {}

    std::string getID() const override { return "A09_"; }

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