#ifndef EXRT_PSEUDOSYNCREADER_H
#define EXRT_PSEUDOSYNCREADER_H

#include <algorithm>
#include <atomic>

#include "ReaderPort.h"
#include "common/Reader_ifs.h"
#include "convtemplate/PrmBuffer_ifs.h"
class IntervalBuffer;

class READER_API_ PseudoSyncPrmBuffer : public PrmBuffer_ifs {
   public:
    typedef double bufferType_t;

    PseudoSyncPrmBuffer(const Parameter_ifs *parent,
                        size_t sample_frequency_log_2,  //
                        double factor = 1, bool is_signed = true,
                        bufferType_t *init_buffer = nullptr,         //
                        size_t intervals_buffer_length = (1 << 12),  //
                        RelativeTime buffer_interval = {0, 30}       //
    );

    ~PseudoSyncPrmBuffer() override;

   private:
    bool update(char *data, size_t number_of_intervals);

    void increasePoints(size_t number_of_points, IntervalBuffer *ir);

   public:
    Reader_ifs *createReader() override;

    void setData(char *data, size_t length, DataStatus status) override;

    Borders getAvailableBorders();

    bool lock(bool arg) override;

    bool isLock() override;

   protected:
    double factor_;
    bool is_signed_;

    std::atomic_bool updating_borders_ = false;

    std::atomic_bool is_locked_ = false;
    std::atomic_bool set_data_in_progress_ = false;

    const size_t sample_frequency_log_2_;

    bufferType_t *buffer_ = nullptr;
    const size_t buffer_length_;
    size_t buffer_start_pos_ = 0;

   public:
    const size_t buffer_time_step_;

   protected:
    /* time from which the buffer contains intervals */
    AbsoluteTime buffer_start_time_ = AbsoluteTime();

    /* time interval for which the "buffer_" contains values */
    const RelativeTime buffer_time_interval_;

    IntervalBuffer **ir_buffers_ = nullptr;

    size_t seed_intervals_length_ = 0;

    double getIntervalsBufferStep(IntervalBuffer *current);

    size_t getStartIndex(const Borders &borders) const;

    size_t getEndIndex(const Borders &borders) const;

    size_t getIntervalCapacity(const Borders &borders) const;

    void addToPoint(Reader_ifs::Point &p, size_t index);

    void initPoint(Reader_ifs::Point &p, size_t index);

    friend class IntervalBuffer;
    //#ifdef UNIT_TEST
    friend class UnitTestPseudoSyncPrmBufferFriend;
    //#endif
    bool initBuffer(char *data, size_t number_of_points);
};

inline void PseudoSyncPrmBuffer::initPoint(Reader_ifs::Point &p, size_t index) {
    p.sum = buffer_[index];
    p.max = p.sum;
    p.min = p.sum;
}

inline void PseudoSyncPrmBuffer::addToPoint(Reader_ifs::Point &p, size_t index) {
    Reader_ifs::readerData_t val = buffer_[index];
    p.sum += val;
    p.max = val > p.max ? val : p.max;  // std::max(val, p.max);
    p.min = val < p.min ? val : p.min;
    // p.min = std::min(val, p.min);
}

inline size_t PseudoSyncPrmBuffer::getStartIndex(const Borders &borders) const {
    int64_t distance = (borders.begin - buffer_start_time_).time;
    return size_t(distance >> uint64_t(32 - sample_frequency_log_2_));
}

inline size_t PseudoSyncPrmBuffer::getEndIndex(const Borders &borders) const {
    int64_t distance = (borders.end - buffer_start_time_).time;
    return size_t(distance >> uint64_t(32 - sample_frequency_log_2_));
}

inline size_t PseudoSyncPrmBuffer::getIntervalCapacity(const Borders &borders) const {
    size_t begin = getStartIndex(borders);
    size_t end = getEndIndex(borders);
    return end - begin;
}

#endif  // EXRT_PSEUDOSYNCREADER_H
