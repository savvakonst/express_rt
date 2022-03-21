

#include "Module_A01_.h"

#include "convtemplate/PrmBuffer_ifs.h"

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

Module_A01_::~Module_A01_() = default;

const DataSchema_ifs* Module_A01_::getPropertySchema() const { return nullptr; }

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
#include "iostream"
bool Module_A01_::isChannelAvailable(const std::string& prop_path) const {
    auto hd = getProperty("cnl/" + prop_path + "/frequency");
    if ((hd == nullptr) || !hd->isValue()) {
        std::cout << "path not exists: header/cnl/" + prop_path + "/frequency\n";
        return false;
    }
    return hd->getValue().value_.u8 != 0xff;
}

ModuleStream_ifs* Module_A01_::createModuleStream() {
    // error_mesadge_ = "The createModuleStream function is not realised yet";
    // TODO:
    if (ethernet_stream_ == nullptr) ethernet_stream_ = new EthernetA01_Stream(this);
    return ethernet_stream_;
};

const ErrorInfo_ifs* Module_A01_::getErrorInfo() const { return nullptr; }

/*
 *
 *
 *
 */

#include <intrin.h>

#pragma intrinsic(_BitScanForward)

unsigned char scanForwardBits(char max_val, unsigned long arg) {
    unsigned long index;
    unsigned char b = _BitScanForward(&index, arg);
    index = max_val - index;
    if (!b) index = 0;
    return (unsigned char)index;
}

#define new_arr(TYPE, LEN) new TYPE[(LEN)]

static std::vector<unsigned char> getList(const std::vector<int>& vec) {
    /* parse arguments */

    int int_list_len = 0;

    int_list_len = (int)vec.size();
    if (int_list_len <= 0) return {};

    long* int_arr = new_arr(long, int_list_len);

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
    unsigned long freq_arr_size = int_list_len;
    long* freq_arr = int_arr;
    //////////////////

    max_e -= min_e;
    long idx_arr_size = 0;
    long time_ponts_size = 1 << (char)max_e;

    for (long *e_ptr = freq_arr, *e_ptr_end = (freq_arr + freq_arr_size); e_ptr < e_ptr_end; e_ptr++) {
        *e_ptr -= min_e;
        idx_arr_size += 1 << (char)*e_ptr;
    }

    std::vector<unsigned char> ret_vector(idx_arr_size, 0);
    ret_vector.reserve(idx_arr_size);
    auto* idx_arr = ret_vector.data();

    {  // isolated namespace
        unsigned char* e_ptr = idx_arr;
        for (long pos = 0; pos < time_ponts_size; pos++) {
            unsigned char pos_val = scanForwardBits((char)max_e, pos);
            for (unsigned char fr_idx = 0; fr_idx < (unsigned char)freq_arr_size; fr_idx++) {
                if (pos_val <= freq_arr[fr_idx]) {
                    *(e_ptr++) = fr_idx;
                }
            }
        }
    }
    delete[] int_arr;

    return ret_vector;
}

EthernetA01_Stream::EthernetA01_Stream(Module_A01_* module) : module_(module) {
    // TODO: need to choose available modules
    std::vector<int> active_freq;

    std::string name = "A01_[" + std::to_string(module_->task_.header.slot) + "]";

    buffers_ = new double*[32];
    size_buffers_ = new size_t[32];
    current_buffers_ = new double*[32];

    auto temp_buffers = buffers_;
    auto temp_size_buffers = size_buffers_;
    auto temp_current_buffers = current_buffers_;

    for (auto& i : module_->task_.cnl) {
        int fr = i.frequency;

        if (0xff != fr) {
            active_freq.push_back(fr);

            auto buffer = nullptr;
            prm_buff_vec_.push_back(buffer);

            auto temp_size = 1 << (fr - 10);

            auto* d_ptr = new double[temp_size];
            *(temp_buffers++) = d_ptr;
            *(temp_size_buffers++) = temp_size;
            *(temp_current_buffers++) = d_ptr;
        }
    }

    vec_ = getList(active_freq);
    prm_buff_end_ptr_ = prm_buff_vec_.data() + prm_buff_vec_.size();
    channels_map_ = vec_.data();
    current_ptr_ = channels_map_;
    channels_map_end_ = channels_map_ + vec_.size();
}

void EthernetA01_Stream::readFramePeace(ModuleStreamContext_ifs* context, char* ptr, size_t size) {
    auto* c_ptr = (int16_t*)ptr;
    int16_t* end_ptr = c_ptr + size;

    unsigned char* map_ptr = current_ptr_;

    auto temp_buffers = buffers_;
    auto temp_size_buffers = size_buffers_;
    auto temp_current_buffers = current_buffers_;

    while (c_ptr < end_ptr) {
        auto k = *(map_ptr++);
        *((temp_current_buffers[k])++) = double(*(c_ptr++)) / (1 << 16);

        if (map_ptr == channels_map_end_) map_ptr = channels_map_;
    }

    temp_current_buffers = current_buffers_;

    auto temp_prm = prm_buff_vec_.data();
    while (temp_prm <= prm_buff_end_ptr_) {
        *(temp_current_buffers++) = *temp_buffers;
        if (temp_prm) (*temp_prm)->setData((char*)*(temp_buffers), *(temp_size_buffers), DataStatus::success);
        (temp_prm++, temp_buffers++, temp_size_buffers++);
    }

    current_ptr_ = map_ptr;
}

const Module_ifs* EthernetA01_Stream::getModule() { return module_; }

bool EthernetA01_Stream::addPrmBuffer(const std::string& path, PrmBuffer_ifs* prm_buffer) {
    auto cnl = std::stoul(path);
    if ((cnl >= 32) || (module_->task_.cnl[cnl].frequency == 0xff)) return false;

    int cnt = 0;

    for (size_t i = 0; i < cnl; i++) {
        auto fr = module_->task_.cnl[i].frequency;
        if (0xff != fr) cnt++;
    }
    prm_buff_vec_[cnt] = prm_buffer;
    return true;
}

std::map<std::string, PrmBuffer_ifs*> EthernetA01_Stream::getPrmBufferMap() {
    std::map<std::string, PrmBuffer_ifs*> prm_buffer_map;

    auto cnt = 0;
    for (size_t i = 0; i < 32; i++) {
        int fr = (int)module_->task_.cnl[i].frequency;
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
