//
// Created by SVK on 24.03.2022.
//

#ifndef EXRT_COMMON_H
#define EXRT_COMMON_H

#include <string>
#include <vector>

#include "convtemplate/PrmBuffer_ifs.h"
#include "device/ModuleStream_ifs.h"

std::vector<unsigned char> createChannelsMap(const std::vector<int>& vec);

const uint8_t kBaudRateLog2 = 10;

template <typename ModuleClass>

class EthernetSyncXXXX_Stream : public ModuleStream_ifs {
#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))
   public:
    explicit EthernetSyncXXXX_Stream(ModuleClass* module) : module_(module) {
        // TODO: need to choose available modules
        std::vector<int> active_freq;
        auto& task = module_->getTask();
        const size_t channels_number = COUNT_OF(task.cnl);

        std::string name = module_->getID() + "[" + std::to_string(task.header.slot) + "]";

        buffers_ = new targetType_t*[channels_number];
        size_buffers_ = new size_t[channels_number];
        current_buffers_ = new targetType_t*[channels_number];

        auto temp_buffers = buffers_;
        auto temp_size_buffers = size_buffers_;
        auto temp_current_buffers = current_buffers_;

        // frequency of packet
        decltype(task.cnl->frequency) min_frequency_log_2 = kBaudRateLog2;

        for (auto& i : task.cnl)
            if (min_frequency_log_2 > i.frequency) min_frequency_log_2 = i.frequency;

        write_every_cycle_ = min_frequency_log_2 == kBaudRateLog2;

        for (auto& i : task.cnl) {
            int fr = i.frequency;

            if (0xff != fr) {
                active_freq.push_back(fr);

                prm_buff_vec_.push_back(nullptr);

                size_t temp_size = 1 << (fr - min_frequency_log_2);

                auto* d_ptr = new targetType_t[temp_size];
                *(temp_buffers++) = d_ptr;
                *(temp_size_buffers++) = temp_size;
                *(temp_current_buffers++) = d_ptr;
            }
        }

        vec_ = createChannelsMap(active_freq);
        prm_buff_end_ptr_ = prm_buff_vec_.data() + prm_buff_vec_.size();
        channels_map_ = vec_.data();
        current_ptr_ = channels_map_;
        channels_map_end_ = channels_map_ + vec_.size();
    }

    ~EthernetSyncXXXX_Stream() override = default;

    void readFramePeace(ModuleStreamContext_ifs* context, char* ptr, size_t size) override {
        auto* c_ptr = (int16_t*)ptr;
        int16_t* end_ptr = c_ptr + size;

        unsigned char* map_ptr = current_ptr_;

        auto temp_buffers = buffers_;
        auto temp_size_buffers = size_buffers_;
        auto temp_current_buffers = current_buffers_;

        while (c_ptr < end_ptr) {
            auto k = *(map_ptr++);
            *((temp_current_buffers[k])++) = targetType_t(*(c_ptr++));  /// (1 << 16);

            if (map_ptr == channels_map_end_) map_ptr = channels_map_;
        }

        current_ptr_ = map_ptr;

        if(!write_every_cycle_ && map_ptr!=channels_map_)
            return;

        temp_current_buffers = current_buffers_;

        auto temp_prm = prm_buff_vec_.data();
        while (temp_prm < prm_buff_end_ptr_) {
            *(temp_current_buffers++) = *temp_buffers;
            if (*temp_prm) {
                (*temp_prm)->setData((char*)*(temp_buffers), *(temp_size_buffers), DataStatus::success);
            }
            (temp_prm++, temp_buffers++, temp_size_buffers++);
        }
    }

    int getStatistic() override {
        // TODO:
        return 0;
    }

    const Module_ifs* getModule() override { return module_; }

    bool addPrmBuffer(const std::string& path, PrmBuffer_ifs* prm_buffer) override {
        auto& task = module_->getTask();
        const size_t channels_number = COUNT_OF(task.cnl);

        auto cnl = std::stoul(path);

        if ((cnl >= channels_number) || (task.cnl[cnl].frequency == 0xff)) return false;

        int cnt = 0;

        for (size_t i = 0; i < cnl; i++) {
            auto fr = task.cnl[i].frequency;
            if (0xff != fr) cnt++;
        }

        prm_buff_vec_[cnt] = prm_buffer;
        return true;
    }

    std::map<std::string, PrmBuffer_ifs*> getPrmBufferMap() override {
        std::map<std::string, PrmBuffer_ifs*> prm_buffer_map;
        auto& task = module_->getTask();
        const size_t channels_number = COUNT_OF(task.cnl);

        auto cnt = 0;
        for (size_t i = 0; i < channels_number; i++) {
            int fr = (int)task.cnl[i].frequency;
            if (0xff != fr) {
                if (prm_buff_vec_[cnt]) {
                    std::string channel = std::to_string(i);
                    prm_buffer_map[channel] = prm_buff_vec_[cnt];
                }
                cnt++;
            }
        }
        return prm_buffer_map;
    }

   protected:
    typedef int16_t targetType_t;

    ModuleClass* module_ = nullptr;
    std::vector<unsigned char> vec_;

    //
    std::vector<PrmBuffer_ifs*> prm_buff_vec_;
    PrmBuffer_ifs** prm_buff_end_ptr_ = nullptr;
    // size_t size_ = 0;

    unsigned char* channels_map_ = nullptr;
    unsigned char* channels_map_end_ = nullptr;
    unsigned char* current_ptr_ = nullptr;

    targetType_t** buffers_ = nullptr;
    size_t* size_buffers_ = nullptr;
    targetType_t** current_buffers_ = nullptr;

    bool write_every_cycle_ = true;

#undef COUNT_OF
};

#endif  // EXRT_COMMON_H
