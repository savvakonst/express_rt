
#ifndef EXRT_KSDMMODULEC06__H
#define EXRT_KSDMMODULEC06__H

#include <cstdint>
#include <map>

#include "KSDModule_ifs.h"
#include "device/ModuleStream_ifs.h"

class Module_C06_ : public KSDModule_ifs {
   protected:
#pragma pack(1)

    struct Task {
        MODULE_HEADER header;
        uint8_t inputs;
        uint8_t reserved[31];
    };

#pragma pop()

    Task task_;
    TaskMapper field_map_;

   public:
    Module_C06_() : field_map_(TaskMapper({{"header", u32}, {"inputs", u8}, {"reserved", {31, u8}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_C06_(void* ptr, size_t size) : Module_C06_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_C06_() {}

    std::string getID() const override { return "C06_"; }

    const InfoList& getPropertiesInfoList() override;

    const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
