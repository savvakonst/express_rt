
#ifndef EXRT_KSDMMODULEA10__H
#define EXRT_KSDMMODULEA10__H

#include <cstdint>
#include <map>

#include "KSDModule_ifs.h"
#include "device/ModuleStream_ifs.h"

class Module_A10_ : public KSDModule_ifs {
   protected:
#pragma pack(1)

    struct CHANNEL_A_10 {
        uint8_t frequency;
        uint8_t frequency2;
        uint8_t frequency3;
        uint8_t flags;
        int32_t range;
        int32_t waves;
        int32_t hysteresis;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_10 cnl[18];
        uint8_t reserved[224];
    };

#pragma pop()

    Task task_;
    TaskMapper field_map_;

   public:
    Module_A10_()
        : field_map_(TaskMapper({{"header", u32},
                                 {"cnl",
                                  {18, TaskMapper({{"frequency", u8},
                                                   {"frequency2", u8},
                                                   {"frequency3", u8},
                                                   {"flags", u8},
                                                   {"range", i32},
                                                   {"waves", i32},
                                                   {"hysteresis", i32}})}},
                                 {"reserved", {224, u8}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_A10_(void* ptr, size_t size) : Module_A10_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_A10_() {}

    std::string getID() const override { return "A10_"; }

    const InfoList& getPropertiesInfoList() override;

    const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
