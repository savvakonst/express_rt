
#ifndef EXRT_KSDMMODULEC06M_H
#define EXRT_KSDMMODULEC06M_H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_C06M : public KSDModule {
   protected:
#pragma pack(1)

    struct Task {
        MODULE_HEADER header;
        uint8_t inputs;
        uint8_t reserved[31];
    };

#pragma pop()

    Task task_;

   public:
    Module_C06M()
        : KSDModule(  //
              TaskMapper({{"header", header_map_}, {"inputs", u8}, {"reserved", {31, u8}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_C06M(void* ptr, size_t size) : Module_C06M() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_C06M() {}

    std::string getID() const override { return "C06M"; }

    // const InfoList& getPropertiesInfoList() override;

    // const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    // const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    // bool setProperty(const std::string& prop_path, const Value value) override;
    // bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
