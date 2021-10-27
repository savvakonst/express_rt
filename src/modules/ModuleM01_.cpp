
#ifndef EXRT_KSDMMODULEM01__H
#define EXRT_KSDMMODULEM01__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_M01_ : public KSDModule {
   protected:
#pragma pack(1)

    struct ETHERNET_ADDRESS {
        uint8_t mac[6];
        uint8_t port[2];
        uint8_t ip_b[4];
        uint8_t reserved[4];
    };

    struct SETTINGS_OUT_ETHERNET {
        uint32_t flags;
        ETHERNET_ADDRESS dst;
        ETHERNET_ADDRESS src;
    };

    struct Task {
        MODULE_HEADER header;
        uint8_t frequency;
        uint8_t reserved_a[2];
        uint8_t flags;
        SETTINGS_OUT_ETHERNET out;
        uint32_t slave_id;
        uint8_t reserved_b[124];
    };

#pragma pop()

    Task task_;

   public:
    Module_M01_()
        : KSDModule(  //
              TaskMapper(
                  {{"header", i32},
                   {"frequency", u8},
                   {"reserved_a", {2, u8}},
                   {"flags", u8},
                   {"out",
                    TaskMapper(
                        {{"flags", u32},
                         {"dst",
                          TaskMapper({{"mac", {6, u8}}, {"port", {2, u8}}, {"ip_b", {4, u8}}, {"reserved", {4, u8}}})},
                         {"src", TaskMapper({{"mac", u8}, {"port", u8}, {"ip_b", u8}, {"reserved", u8}})}})},
                   {"slave_id", u32},
                   {"reserved_b", {124, u8}}})) {
        field_map_.setReferencePtr(&task_);
    }

    Module_M01_(void* ptr, size_t size) : Module_M01_() {
        if (size != getTaskSize()) {
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }

    ~Module_M01_() {}

    std::string getID() const override { return "M01_"; }

    // const InfoList& getPropertiesInfoList() override;

    // const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    // const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    // bool setProperty(const std::string& prop_path, const Value value) override;
    // bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }
};

#endif
