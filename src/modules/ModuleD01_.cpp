
#ifndef EXRT_KSDMMODULED01__H
#define EXRT_KSDMMODULED01__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_D01_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_COUNTER {
        uint8_t flags;
        int32_t voltageP;
        int32_t voltageN;
        int32_t filter;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_COUNTER cnl[16];
    };

#pragma pop()

    Task task_;

   public:
    Module_D01_()
        : KSDModule(  //
              TaskMapper(
                  {{"header", header_map_},
                   {"cnl",
                    {16, TaskMapper({{"flags", u8}, {"voltageP", i32}, {"voltageN", i32}, {"filter", i32}})}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_D01_(void* ptr, size_t size) : Module_D01_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_D01_() {}

    std::string getID() const override { return "D01_"; }

    // const InfoList& getPropertiesInfoList() override;

    // const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    // const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    // bool setProperty(const std::string& prop_path, const Value value) override;
    // bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
