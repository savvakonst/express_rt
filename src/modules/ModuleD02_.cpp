
#ifndef EXRT_KSDMMODULED02__H
#define EXRT_KSDMMODULED02__H

#include <cstdint>
#include <map>

#include "KSDModule_ifs.h"
#include "device/ModuleStream_ifs.h"

class Module_D02_ : public KSDModule_ifs {
   protected:
#pragma pack(1)

    struct CHANNEL_D {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t filter;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_D cnl[24];
    };

#pragma pop()

    Task task_;
    TaskMapper field_map_;

   public:
    Module_D02_()
        : field_map_(TaskMapper(
              {{"header", u32},
               {"cnl",
                {24, TaskMapper({{"frequency", u8}, {"reserved", {2, u8}}, {"flags", u8}, {"filter", i32}})}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_D02_(void* ptr, size_t size) : Module_D02_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_D02_() {}

    std::string getID() const override { return "D02_"; }

    const InfoList& getPropertiesInfoList() override;

    const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
