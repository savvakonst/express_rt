
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

#pragma pop()

    Task task_;

   public:
    Module_A05_()
        : KSDModule(  //
              TaskMapper({{"header", header_map_},
                          {"cnl",
                           {9, TaskMapper({{"frequency", u8},
                                           {"reserved", {2, u8}},
                                           {"flags", u8},
                                           {"range", i32},
                                           {"ureserved", {2, u32}}})}},
                          {"reserved", {112, u8}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_A05_(void* ptr, size_t size) : Module_A05_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_A05_() {}

    std::string getID() const override { return "A05_"; }

    // const InfoList& getPropertiesInfoList() override;

    // const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    // const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    // bool setProperty(const std::string& prop_path, const Value value) override;
    // bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
