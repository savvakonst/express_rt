
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
    Module_C04_()
        : KSDModule(  //
              TaskMapper({{"header", header_map_},
                          {"active", u32},
                          {"can", {6, TaskMapper({{"frequency", u32}})}},
                          {"rs",
                           {12, TaskMapper({{"frequency", u32},
                                            {"dataLength", u8},
                                            {"stopLength", u8},
                                            {"parity", u8},
                                            {"typeOfCode", u8}})}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_C04_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_C04_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_C04_() {}

    std::string getID() const override { return "C04_"; }

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
