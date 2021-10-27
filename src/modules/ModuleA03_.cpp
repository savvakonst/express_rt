
#ifndef EXRT_KSDMMODULEA03__H
#define EXRT_KSDMMODULEA03__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A03_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_A_03 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t range;
        int32_t current;
        int32_t offset;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_03 cnl[16];
    };

#pragma pop()

    Task task_;

   public:
    Module_A03_()
        : KSDModule(  //
              TaskMapper({{"header", header_map_},
                          {"cnl",
                           {16, TaskMapper({{"frequency", u8},
                                            {"reserved", {2, u8}},
                                            {"flags", u8},
                                            {"range", i32},
                                            {"current", i32},
                                            {"offset", i32}})}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_A03_(void* ptr, size_t size) : Module_A03_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_A03_() {}

    std::string getID() const override { return "A03_"; }

    // const InfoList& getPropertiesInfoList() override;

    // const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    // const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    // bool setProperty(const std::string& prop_path, const Value value) override;
    // bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
