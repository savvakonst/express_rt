

#include "Module_A01_.h"

Module_A01_::Module_A01_()
    : KSDModule(  //
          TaskMapper({{"header", header_map_},
                      {"cnl",
                       {32, TaskMapper({{"frequency", u8},
                                        {"reserved", {2, u8}},
                                        {"flags", u8},
                                        {"rangeH", i32},
                                        {"rangeL", i32},
                                        {"ureserved", u32}})}}})) {
    field_map_.setReferencePtr(&task_);
}

Module_A01_::Module_A01_(const void* ptr, size_t size, ExtensionManager* context) : Module_A01_() {
    if (size != getTaskSize()) {
        // print error or throw error
    }
    task_ = *((Task*)ptr);
}

Module_A01_::~Module_A01_() {}

const DataSchema_ifs* Module_A01_::getPropertySchema() { return nullptr; }

const HierarchicalData_ifs* Module_A01_::getProperty(const std::string& prop_path) const {
    return KSDModule::getProperty(prop_path);
}

std::string Module_A01_::getPropertyAsTxt(const std::string& prop_path) const {
    return KSDModule::getPropertyAsTxt(prop_path);
}

bool Module_A01_::setProperty(const std::string& prop_path, const Value& value) {
    return KSDModule::setProperty(prop_path, value);
}
bool Module_A01_::setPropertyAsTxt(const std::string& prop_path, const std::string& value) {
    return KSDModule::setPropertyAsTxt(prop_path, value);
}

ModuleStream_ifs* Module_A01_::createModuleStream() {
    // error_mesadge_ = "The createModuleStream function is not realised yet";
    // TODO:
    if (ethernet_stream_ == nullptr) ethernet_stream_ = new EthernetA01_Stream(this);
    return ethernet_stream_;
};

const ErrorInfo_ifs* Module_A01_::getErrorInfo(void) const { return nullptr; }

/*
 *
 *
 *
 */

#include <intrin.h>

#include "Reader/IntervalBuffer.h"
#include "Reader/PseudoSyncPrmBuffer.h"
#pragma intrinsic(_BitScanForward)

char scanForwardBits(char max_val, unsigned long arg) {
    unsigned long index;
    char b = _BitScanForward(&index, arg);
    index = max_val - index;
    if (!b) index = 0;
    return (unsigned char)index;
}

#define new_arr(TYPE, LEN) new TYPE[(LEN)]

static std::vector<char> getList(const std::vector<int>& vec) {
    /* parse arguments */

    int int_list_len = 0;

    int_list_len = (int)vec.size();
    if (int_list_len <= 0) return std::vector<char>();

    long* int_arr = new_arr(long, int_list_len);
    if (int_arr == NULL) return std::vector<char>();

    int max_e = 0;
    int min_e = vec[0];
    for (int idx = 0; idx < int_list_len; idx++) {
        int e = vec[idx];
        if (max_e < e) max_e = e;
        if (e < min_e) min_e = e;

        int_arr[idx] = e;
    }

    //////////////////
    // redefine
    long freq_arr_size = int_list_len;
    long* freq_arr = int_arr;
    //////////////////

    max_e -= min_e;
    long idx_arr_size = 0;
    long time_ponts_size = 1 << (char)max_e;

    for (long *e_ptr = freq_arr, *e_ptr_end = (freq_arr + freq_arr_size); e_ptr < e_ptr_end; e_ptr++) {
        *e_ptr -= min_e;
        idx_arr_size += 1 << (char)*e_ptr;
    }

    char* idx_arr = new_arr(char, idx_arr_size);

    {  // isolated namespace
        char* e_ptr = idx_arr;
        for (long pos = 0; pos < time_ponts_size; pos++) {
            char pos_val = scanForwardBits((char)max_e, pos);
            for (char fr_idx = 0; fr_idx < freq_arr_size; fr_idx++) {
                if (pos_val <= freq_arr[fr_idx]) {
                    *(e_ptr++) = fr_idx;
                }
            }
        }
    }
    delete[] int_arr;

    std::vector<char> python_val;
    python_val.reserve(idx_arr_size);
    for (int idx = 0; idx < idx_arr_size; idx++) python_val.push_back((char)idx_arr[idx]);

    delete[] idx_arr;
    return python_val;
}

EthernetA01_Stream::EthernetA01_Stream(Module_A01_* module) : module_(module) {
    // TODO: need to choose available modules
    std::vector<int> active_freq;

    std::string name = "A01_[" + std::to_string(module->task_.header.slot) + "]";

    buffers_ = new double*[32];
    size_buffers_ = new size_t[32];
    current_buffers_ = new double*[32];

    auto temp_buffers_ = buffers_;
    auto temp_size_buffers_ = size_buffers_;
    auto temp_current_buffers_ = current_buffers_;

    for (size_t i = 0; i < 32; i++) {
        int fr = (int)module->task_.cnl[i].frequency;

        if (0xff != fr) {
            active_freq.push_back(fr);

            auto buffer = new PseudoSyncPrmBuffer(nullptr, fr, nullptr);
            std::string channel = ", канал: " + std::to_string(i);
            prm_buff_vec_.push_back(buffer);
            prm_buffer_map_[name + channel] = buffer;

            auto temp_size = 1 << (fr - 10);

            double* d_ptr = new double[temp_size];
            *(temp_buffers_++) = d_ptr;
            *(temp_size_buffers_++) = temp_size;
            *(temp_current_buffers_++) = d_ptr;
        }
    }

    vec_ = getList(active_freq);
    prm_buff_ptr_ = prm_buff_vec_.data();
    prm_buff_end_ptr_ = prm_buff_ptr_ + prm_buff_vec_.size();
    channels_map_ = vec_.data();
    current_ptr_ = channels_map_;
    channels_map_end_ = channels_map_ + vec_.size();
}
#include <iostream>
void EthernetA01_Stream::readFramePeace(ModuleStreamContext_ifs* context, char* ptr, size_t size) {
    int16_t* c_ptr = (int16_t*)ptr;
    int16_t* end_ptr = c_ptr + size;

    char* map_ptr = current_ptr_;

    auto temp_buffers_ = buffers_;
    auto temp_size_buffers_ = size_buffers_;
    auto temp_current_buffers_ = current_buffers_;

    // std::cout<<"ret";
    while (c_ptr < end_ptr) {
        // prm_buff_ptr_[*(map_ptr)]->setData(ptr, 1, DataStatus::success);

        auto k = *(map_ptr++);
        *((temp_current_buffers_[k])++) = double(*(c_ptr++)) / (1 << 16);

        if (map_ptr == channels_map_end_) map_ptr = channels_map_;
    }

    temp_current_buffers_ = current_buffers_;

    auto temp_prm = prm_buff_ptr_;
    while (temp_prm < prm_buff_end_ptr_) {
        *(temp_current_buffers_++) = *temp_buffers_;
        (*temp_prm++)->setData((char*)*(temp_buffers_++), *(temp_size_buffers_++), DataStatus::success);
    }

    current_ptr_ = map_ptr;
}

std::map<std::string, PrmBuffer_ifs*> EthernetA01_Stream::getPrmBufferMap() { return prm_buffer_map_; }
