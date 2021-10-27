
#ifndef EXRT_KSDMMODULEID02_H
#define EXRT_KSDMMODULEID02_H

#include <cstdint>
#include <map>

#include "KSDModule_ifs.h"
#include "device/ModuleStream_ifs.h"

class Module_ID02 : public KSDModule_ifs {
   protected:
#pragma pack(1)

    struct IMIT_CHANNEL_D {
        uint8_t state;
        uint8_t reservedA;
        uint16_t onCode;
        uint16_t offCode;
        uint16_t width;
        uint16_t frequency;
        uint8_t record;
        uint8_t reservedB;
    };

    struct Task {
        MODULE_HEADER header;
        IMIT_CHANNEL_D out[2];
    };

#pragma pop()

    Task task_;
    TaskMapper field_map_;

   public:
    Module_ID02()
        : field_map_(TaskMapper({{"header", u32},
                                 {"out",
                                  {2, TaskMapper({{"state", u8},
                                                  {"reservedA", u8},
                                                  {"onCode", u16},
                                                  {"offCode", u16},
                                                  {"width", u16},
                                                  {"frequency", u16},
                                                  {"record", u8},
                                                  {"reservedB", u8}})}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_ID02(void* ptr, size_t size) : Module_ID02() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_ID02() {}

    std::string getID() const override { return "ID02"; }

    const InfoList& getPropertiesInfoList() override;

    const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
