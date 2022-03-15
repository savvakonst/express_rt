
#ifndef EXRT_KSDMMODULEM01__H
#define EXRT_KSDMMODULEM01__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_M01_;

class EthernetM01_Stream : public ModuleStream_ifs {
   public:
    EthernetM01_Stream(Module_M01_* module);

    ~EthernetM01_Stream() {}

    void readFramePeace(ModuleStreamContext_ifs* context, char* ptr, size_t size) override;

    int getStatistic() override {
        // TODO:

        return 0;
    }

    const Module_ifs* getModule() override { return module_; }

   protected:
    Module_ifs* module_;
};

class Module_M01_ : public KSDModule {
    friend EthernetM01_Stream;

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

#pragma pack()

    Task task_;

    EthernetM01_Stream* ethernet_stream_ = nullptr;

   public:
    Module_M01_();

    Module_M01_(const void* ptr, size_t size, ExtensionManager* manager);

    ~Module_M01_();

    bool hasTransceiver() const override {
        bool b = (task_.out.flags & 0x01) == 0x01;
        return b;
    }

    EthernetSettings getSrcAddress() const override {
        if (!hasTransceiver()) return EthernetSettings();
        EthernetSettings e{*((EthernetAddress*)&task_.out.dst), *((EthernetAddress*)&task_.out.src)};
        return e;
    }

    std::string getID() const override { return "M01_"; }

    const DataSchema_ifs* getPropertySchema() const override;

    const HierarchicalData_ifs* getProperty(const std::string& prop_path) const override;
    std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value& value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    bool storeTaskToBuffer(void* pointer) const override {
        memcpy(pointer, (void*)&task_, sizeof(Task));
        return true;
    }
    size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override;

    const ErrorInfo_ifs* getErrorInfo(void) const override;
};

#endif
