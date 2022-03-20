

#define _USE_MATH_DEFINES
#include "Receiver/Receiver.h"
//
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "Reader/IntervalBuffer.h"
#include "Reader/PseudoSyncPrmBuffer.h"
#include "ReaderExample.h"

template <typename T>
struct VectorStr {
    T* list_;
    size_t len_;
};

struct ParameterProp {
    PseudoSyncPrmBuffer* reader_;
    size_t time_step_;
    size_t cnt;
    double signal_frequency_;
    double* data_;
};

template <typename T>
void addToList(T thr_ptr, VectorStr<T>* vec) {
    T* new_list_ = new T[vec->len_ + 1];
    for (size_t idx = 0; idx < vec->len_; idx++) new_list_[idx] = vec->list_[idx];
    if (vec->list_) delete[] vec->list_;

    vec->list_ = new_list_;
    vec->list_[vec->len_] = thr_ptr;
    vec->len_++;
}

size_t g_update_period = 10;  // milliseconds
std::thread* g_thread = nullptr;
VectorStr<ParameterProp*> g_prop_list = {nullptr, 0};
std::atomic_int32_t g_cmd;

inline double getSignalPoint(double x) { return std::sin(2. * M_PI * x) * std::sin(2. * M_PI * x * 50); }

static void updateData_(VectorStr<ParameterProp*> prop_list, size_t time_, size_t update_period) {
    auto len = prop_list.len_;
    auto list = prop_list.list_;

    for (size_t idx = 0; idx < len; idx++) {
        list[idx]->data_ = new double[list[idx]->cnt];
    }

    while (g_cmd == 0) {
        size_t time = time_;

        for (size_t idx = 0; idx < len; idx++) {
            ParameterProp* pr = list[idx];

            PseudoSyncPrmBuffer* reader_ = pr->reader_;
            size_t time_step_ = pr->time_step_;
            size_t cnt = pr->cnt;
            double signal_frequency_ = pr->signal_frequency_;
            double* data_ = pr->data_;

            time = time_;

            for (size_t i = 0; i < cnt; i++) {
                data_[i] = getSignalPoint(double(time) * signal_frequency_);
                time += time_step_;
            }

            reader_->setData((char*)data_, cnt, DataStatus::success);
        }
        time_ = time;
        std::this_thread::sleep_for(std::chrono::milliseconds(update_period));
    }

    for (size_t idx = 0; idx < len; idx++) {
        delete[] list[idx]->data_;
        list[idx]->data_ = nullptr;
    }
}

static void initData_(VectorStr<ParameterProp*> prop_list, size_t& start_time_, size_t stop_time_,
                      size_t update_period) {
    auto len = prop_list.len_;
    auto list = prop_list.list_;

    for (size_t idx = 0; idx < len; idx++) {
        list[idx]->data_ = new double[list[idx]->cnt];
    }

    while (start_time_ < stop_time_) {
        size_t time = start_time_;

        for (size_t idx = 0; idx < len; idx++) {
            ParameterProp* pr = list[idx];

            PseudoSyncPrmBuffer* reader_ = pr->reader_;
            size_t time_step_ = pr->time_step_;
            size_t cnt = pr->cnt;
            double signal_frequency_ = pr->signal_frequency_;
            double* data_ = pr->data_;

            time = start_time_;

            for (size_t i = 0; i < cnt; i++) {
                data_[i] = getSignalPoint(double(time) * signal_frequency_);
                time += time_step_;
            }

            reader_->setData((char*)data_, cnt, DataStatus::success);
        }
        start_time_ = time;
    }

    for (size_t idx = 0; idx < len; idx++) {
        delete[] list[idx]->data_;
        list[idx]->data_ = nullptr;
    }
}

void removeAllPrmBufferSimulator() {
    g_cmd = 1;
    if (g_thread) {
        g_thread->join();
        delete g_thread;
        g_thread = nullptr;
    }
    auto len = g_prop_list.len_;
    auto list = g_prop_list.list_;

    for (size_t idx = 0; idx < len; idx++) {
        auto p = list[idx];
        if (p->data_) delete p->data_;
        if (p->reader_) delete p->reader_;
    }

    delete list;
    g_prop_list = {nullptr, 0};
}

PrmBuffer_ifs* createNewPrmBufferSimulator(size_t sample_frequency_log2, size_t reader_buffer_length,
                                           RelativeTime buffer_interval, double signal_frequency) {
    // buffer_interval.ls_ineger;
    g_cmd = 1;
    if (g_thread) {
        g_thread->join();
        delete g_thread;
        g_thread = nullptr;
    }

    size_t time_ = 0;

    ParameterProp* pr = new ParameterProp;
    pr->reader_ =
        new PseudoSyncPrmBuffer(nullptr, sample_frequency_log2, nullptr, reader_buffer_length, buffer_interval);
    pr->cnt = size_t(size_t(1) << (sample_frequency_log2 - 10)) * g_update_period;
    pr->data_ = nullptr;
    pr->signal_frequency_ = signal_frequency / (int64_t(1) << 32);
    pr->time_step_ = uint64_t(1) << uint64_t(32 - sample_frequency_log2);

    addToList(pr, &g_prop_list);

    size_t time_end_ = 30 * (uint64_t(1) << uint64_t(32));  // size_t(buffer_interval.time);
    initData_(g_prop_list, time_, time_end_, g_update_period);

    g_cmd = 0;
    g_thread = new std::thread(updateData_, g_prop_list, time_, g_update_period);

    return pr->reader_;
}

Reader_ifs* getReaderExample(PrmBuffer_ifs* arg) {
    PseudoSyncPrmBuffer* pbf = (PseudoSyncPrmBuffer*)(arg);

    while (false == pbf->lock(true)) {
    }
    auto ret = pbf->createIntervalBuffer();
    pbf->lock(false);
    return ret;
}
