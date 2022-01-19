#ifndef EXRT_PSEUDOSYNCREADER_H
#define EXRT_PSEUDOSYNCREADER_H

#include <atomic>

#include "common/Reader_ifs.h"
#include "convtemplate/PrmBuffer_ifs.h"

class IntervalBuffer;

class PseudoSyncPrmBuffer : public PrmBuffer_ifs {
   public:
    typedef double buffer_type_t;

    PseudoSyncPrmBuffer(const Parameter_ifs *parent,
                        size_t sample_frequency_log2,                //
                        buffer_type_t *init_buffer = nullptr,        //
                        size_t intervals_buffer_length = (1 << 12),  //
                        RelativeTime buffer_interval = {0, 30}       //
    );

    ~PseudoSyncPrmBuffer() override;

   private:
    bool update(char *data, size_t number_of_intervals);

    void increasePoints(size_t number_of_points, IntervalBuffer *ir);

   public:
    Reader_ifs *createIntervalBuffer();

    void setData(char *data, size_t length, DataStatus status);

    Borders getAvailableBorders();

    bool lock(bool arg) override;

    bool isLock() override;

   private:
    std::atomic_bool updating_borders_ = false;

    std::atomic_bool is_locked = false;
    std::atomic_bool set_data_in_progress = false;

    const size_t sample_frequency_log2_;

    buffer_type_t *buffer_ = nullptr;
    const size_t buffer_length_;
    size_t buffer_start_pos_ = 0;

   public:
    const size_t buffer_time_step_;

   private:
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
    bool initBuffer(char *data, size_t number_of_points);
};

inline void PseudoSyncPrmBuffer::initPoint(Reader_ifs::Point &p, size_t index) {
    p.sum = buffer_[index];
    p.max = p.sum;
    p.min = p.sum;
}

inline void PseudoSyncPrmBuffer::addToPoint(Reader_ifs::Point &p, size_t index) {
    Reader_ifs::ReaderData val = buffer_[index];
    p.sum += val;
    p.max = std::max(p.max, val);
    p.min = std::min(p.min, val);
}

inline size_t PseudoSyncPrmBuffer::getStartIndex(const Borders &borders) const {
    int64_t distance = (borders.begin - buffer_start_time_).time;
    return size_t(distance >> uint64_t(32 - sample_frequency_log2_));
}

inline size_t PseudoSyncPrmBuffer::getEndIndex(const Borders &borders) const {
    int64_t distance = (borders.end - buffer_start_time_).time;
    return size_t(distance >> uint64_t(32 - sample_frequency_log2_));
}

inline size_t PseudoSyncPrmBuffer::getIntervalCapacity(const Borders &borders) const {
    size_t begin = getStartIndex(borders);
    size_t end = getEndIndex(borders);
    return end - begin;
}

#endif  // EXRT_PSEUDOSYNCREADER_H
