
#ifndef EXRT_KSDMMODULEA01__H
#define EXRT_KSDMMODULEA01__H

#include <cstdint>
#include <map>

#include "KSDModule.h"
#include "device/ModuleStream_ifs.h"

class Module_A01_;

class EthernetA01_Stream : public ModuleStream_ifs {
   public:
    EthernetA01_Stream(Module_A01_* module);

    ~EthernetA01_Stream() {}

    void readFramePeace(ModuleStreamContext_ifs* context, char* ptr, size_t size) override;

    int getStatistic() override {
        // TODO:
        return 0;
    }

    const Module_ifs* getModule() override { return module_; }

    void addPrmBuffer(size_t index, size_t sub_index, PrmBuffer_ifs* prm_buffer) override {}

    std::map<std::string, PrmBuffer_ifs*> getPrmBufferMap() override;

   protected:
    std::map<std::string, PrmBuffer_ifs*> prm_buffer_map_;
    Module_ifs* module_ = nullptr;
    std::vector<char> vec_;
    std::vector<PrmBuffer_ifs*> prm_buff_vec_;
    // size_t size_ = 0;
    PrmBuffer_ifs** prm_buff_ptr_ = nullptr;
    PrmBuffer_ifs** prm_buff_end_ptr_ = nullptr;
    char* channels_map_ = nullptr;
    char* channels_map_end_ = nullptr;
    char* current_ptr_ = nullptr;

    double** buffers_ = nullptr;
    size_t* size_buffers_ = 0;
    double** current_buffers_ = nullptr;
};

class Module_A01_ : public KSDModule {
    friend EthernetA01_Stream;

   protected:
#pragma pack(1)

    struct CHANNEL_A_01 {
        uint8_t frequency;
        uint8_t reserved[2];
        uint8_t flags;
        int32_t rangeH;
        int32_t rangeL;
        uint32_t ureserved;
    };

    struct Task {
        MODULE_HEADER header;
        CHANNEL_A_01 cnl[32];
    };

#pragma pack()

    Task task_;
    EthernetA01_Stream* ethernet_stream_ = nullptr;

   public:
    Module_A01_();

    Module_A01_(const void* ptr, size_t size, DeviceBuildingContext_ifs* context);

    ~Module_A01_();

    // TODO : need to delete
    std::map<std::string, PrmBuffer_ifs*> getPrmBufferMap() override {
        if (ethernet_stream_) return ethernet_stream_->getPrmBufferMap();
        return std::map<std::string, PrmBuffer_ifs*>();
    }

    std::string getID() const override { return "A01_"; }

    const DataSchema_ifs* getPropertySchema() override;

    const HierarchicalData_ifs* getProperty(const std::string& prop_path) const override;
    std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;

    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }

    ModuleStream_ifs* createModuleStream() override;

    const ErrorInfo_ifs* getErrorInfo(void) const override;
};

#endif
