
#ifndef EXRT_KSDMMODULEC03__H
#define EXRT_KSDMMODULEC03__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_C03_ : public KSDModule {
   protected:
#pragma pack(1)

    struct CHANNEL_IRIG {
        uint8_t formatNumber;
        uint8_t dataBits;
        uint8_t parity;
        uint8_t syncBits;
        uint64_t syncWord;
        uint16_t frameLength;
        uint8_t typeOfCode;
        uint8_t reserved;
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
        uint32_t irigMode;
        CHANNEL_IRIG ir[4][4];
        CHANNEL_RS rs[12];
    };

#pragma pack()

    Task task_;

   public:
    Module_C03_()
        : KSDModule(  //
              TaskMapper({{"header", header_map_},
                          {"active", u32},
                          {"irigMode", u32},
                          {"ir",
                           {4,
                            {4, TaskMapper({{"formatNumber", u8},
                                            {"dataBits", u8},
                                            {"parity", u8},
                                            {"syncBits", u8},
                                            {"syncWord", u64},
                                            {"frameLength", u16},
                                            {"typeOfCode", u8},
                                            {"reserved", u8}})}}},
                          {"rs",
                           {12, TaskMapper({{"frequency", u32},
                                            {"dataLength", u8},
                                            {"stopLength", u8},
                                            {"parity", u8},
                                            {"typeOfCode", u8}})}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_C03_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context) : Module_C03_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_C03_() {}

    std::string getID() const override { return "C03_"; }

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
