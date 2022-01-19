//
// Created by SVK on 01.11.2021.
//

#include "PseudoSyncPrmBuffer.h"

#include "IntervalBuffer.h"

inline size_t getLen(void **array) {
    auto len = 0;
    while (*array) {
        array++;
        len++;
    }
    return len;
}

PseudoSyncPrmBuffer::PseudoSyncPrmBuffer(  //
    const Parameter_ifs *parent,           //
    size_t sample_frequency_log2,          //
    buffer_type_t *init_buffer,            //
    size_t intervals_buffer_length,        //
    RelativeTime buffer_interval)
    : sample_frequency_log2_(sample_frequency_log2),
      seed_intervals_length_(intervals_buffer_length),
      buffer_time_interval_(buffer_interval),
      buffer_length_(uint64_t(uint64_t(buffer_interval.time) * uint64_t(uint64_t(1) << sample_frequency_log2_)) >> 32),
      buffer_time_step_(uint64_t(1) << uint64_t(32 - sample_frequency_log2_)),
      PrmBuffer_ifs(parent)  //
{
    buffer_ = new buffer_type_t[buffer_length_];
    buffer_start_pos_ = 0;


    if (init_buffer) {
        buffer_start_time_ = buffer_start_time_ - buffer_time_interval_;
        initBuffer((char *)init_buffer, buffer_length_);
    }

    ir_buffers_ = new IntervalBuffer *[1];
    *ir_buffers_ = nullptr;
    // IntervalBuffer *current = createIntervalBuffer();
    // increasePoints(buffer_length_, current);
}

PseudoSyncPrmBuffer::~PseudoSyncPrmBuffer() {
    delete[] buffer_;

    auto ir_buffers = ir_buffers_;
    while (ir_buffers) delete *(ir_buffers++);
    delete[] ir_buffers;
}

Reader_ifs *PseudoSyncPrmBuffer::createIntervalBuffer() {
    IntervalBuffer *ir = new IntervalBuffer(this, Borders(buffer_start_time_, buffer_time_interval_));

    ir->seed_length_ = seed_intervals_length_;
    ir->borders_ = Borders(buffer_start_time_, buffer_time_interval_);
    ir->data = new Reader_ifs::Point[2 * ir->seed_length_ + 2];
    ir->capacity_ = getIntervalCapacity(ir->borders_);
    ir->step_ = (ir->capacity_ / ir->seed_length_);
    ir->start_pos_ = ir->capacity_ / ir->step_ + 2;
    ir->ord_ = ir->capacity_ % ir->step_;
    ir->length_ = ir->capacity_ / ir->step_ + 2;

    size_t len = ir_buffers_ ? getLen((void **)ir_buffers_) : 0;
    auto new_ir_buffer = new IntervalBuffer *[len + 2];
    auto tmp_new = new_ir_buffer;
    if (ir_buffers_) {
        auto tmp_old = ir_buffers_;
        while (*tmp_old) *(tmp_new++) = *(tmp_old++);
        delete ir_buffers_;
    }
    *(tmp_new++) = ir;
    *tmp_new = nullptr;
    ir_buffers_ = new_ir_buffer;

    increasePoints(buffer_length_, ir);
    return ir;
}

void PseudoSyncPrmBuffer::setData(char *data, size_t length, DataStatus status) {
    set_data_in_progress = true;
    while (is_locked) {
    }

    update(data, length);

    auto ir_buffers = ir_buffers_;

    while (*ir_buffers) increasePoints(length, *(ir_buffers++));

    set_data_in_progress = false;
}

bool PseudoSyncPrmBuffer::initBuffer(char *data, size_t number_of_points) {
    if (number_of_points > buffer_length_) {
        return false;
    }

    size_t first_end_pos = (buffer_start_pos_ + number_of_points);
    size_t second_end_pos = 0;

    if (first_end_pos > buffer_length_) {
        second_end_pos = first_end_pos - buffer_length_;
        first_end_pos = buffer_length_;
    }

    for (size_t i = buffer_start_pos_; i < first_end_pos; i++) {
        buffer_[i] = 0;
    }

    for (size_t i = 0; i < second_end_pos; i++) {
        buffer_[i] = 0;
    }

    int64_t time = buffer_time_step_ * number_of_points;

    // TODO: this is the worst(incorrect) approach to synchronization
    while (updating_borders_)
        ;
    updating_borders_ = true;
    buffer_start_time_ = buffer_start_time_ + (*(RelativeTime *)&time);
    updating_borders_ = false;

    buffer_start_pos_ = buffer_start_pos_ + number_of_points;
    buffer_start_pos_ = EXRT_remainder(buffer_start_pos_, buffer_length_);

    return true;
}

bool PseudoSyncPrmBuffer::update(char *data, size_t number_of_points) {
    if (number_of_points > buffer_length_) {
        return false;
    }

    size_t first_end_pos = (buffer_start_pos_ + number_of_points);
    size_t second_end_pos = 0;

    if (first_end_pos > buffer_length_) {
        second_end_pos = first_end_pos - buffer_length_;
        first_end_pos = buffer_length_;
    }

    buffer_type_t *typed_data = (buffer_type_t *)data;

    for (size_t i = buffer_start_pos_; i < first_end_pos; i++) {
        buffer_[i] = *(typed_data++);
    }

    for (size_t i = 0; i < second_end_pos; i++) {
        buffer_[i] = *(typed_data++);
    }

    int64_t time = buffer_time_step_ * number_of_points;

    // TODO: this is the worst(incorrect) approach to synchronization
    while (updating_borders_)
        ;
    updating_borders_ = true;
    buffer_start_time_ = buffer_start_time_ + (*(RelativeTime *)&time);
    updating_borders_ = false;

    buffer_start_pos_ = buffer_start_pos_ + number_of_points;
    buffer_start_pos_ = EXRT_remainder(buffer_start_pos_, buffer_length_);

    return true;
}

#define INSERT_CODE_()                                                       \
    for (size_t i = ir->buffer_pos_of_right_border_; i < end; i++) {         \
        ir->buffer_pos_of_right_border_ = EXRT_remainder(i, buffer_length_); \
        addToPoint(interval, ir->buffer_pos_of_right_border_);               \
    }                                                                        \
    ir->buffer_pos_of_right_border_++;                                       \
    ir->buffer_pos_of_right_border_ = EXRT_remainder(ir->buffer_pos_of_right_border_, buffer_length_);

void PseudoSyncPrmBuffer::increasePoints(size_t number_of_points, IntervalBuffer *ir) {
    if (number_of_points >= ir->capacity_) {
        // FIXME: be attentive
        ir->buffer_pos_of_right_border_ += (number_of_points - ir->capacity_);
        ir->buffer_pos_of_right_border_ = ir->buffer_pos_of_right_border_ < buffer_length_
                                              ? ir->buffer_pos_of_right_border_
                                              : ir->buffer_pos_of_right_border_ % buffer_length_;

        ir->start_pos_ = 0;
        ir->step_1_ = ir->step_;
        ir->step_2_ = ir->ord_;

        // FIXME: Remove this dummy and determine where the overflow occurs.
        ir->buffer_pos_of_right_border_ = EXRT_remainder(ir->buffer_pos_of_right_border_, buffer_length_);
        size_t last_index = ir->buffer_pos_of_right_border_;

        for (size_t i = 0; i < ir->length_ - 2; i++) {
            last_index = EXRT_remainder(last_index, buffer_length_);
            auto next_index = last_index + ir->step_;

            auto &interval = ir->data[i];

            initPoint(interval, last_index);
            for (size_t j = last_index + 1; (j < next_index); j++) {
                addToPoint(interval, EXRT_remainder(j, buffer_length_));
            }

            // interval.pos.ms_fractional = uint32_t(ir->step_);
            last_index = next_index;
        }

        auto next_index = last_index + ir->ord_;
        auto &interval = ir->data[ir->length_ - 1];
        initPoint(interval, EXRT_remainder(last_index, buffer_length_));
        for (size_t j = last_index + 1; (j < next_index); j++) {
            addToPoint(interval, EXRT_remainder(j, buffer_length_));
        }

        ir->buffer_pos_of_right_border_ = EXRT_remainder(next_index, buffer_length_);

        // bool is_additional_sell = ir->step_2_ >= ir->ord_;
        ir->itrv_ = ir->data + ((ir->start_pos_ + ir->length_ - 2) % ir->length_);
        return;
    }

    // begin calculation
    auto tmp = number_of_points - ir->step_1_ + ir->step_;
    ir->step_1_ = ir->step_ - ((number_of_points < ir->step_1_) ? number_of_points : tmp % ir->step_);

    // end calculation
    bool is_additional_sell = ir->step_2_ >= ir->ord_;
    // TODO: CREATE MEMBER end_pos_ for IntervalBuffer
    auto end_intervals_pos = (ir->start_pos_ + ir->length_ - 1 - size_t(is_additional_sell)) % ir->length_;

    tmp = number_of_points + ir->step_2_;
    auto right_step = tmp % ir->step_;

    /*


    */
    auto g_pos = ir->buffer_pos_of_right_border_;

#define INCREASE_G_POS() \
    g_pos++;             \
    g_pos = EXRT_remainder(g_pos, buffer_length_)

    auto step = ir->step_;
    auto g_end = ir->step_2_ + number_of_points;
    auto k = ir->step_2_;

    auto itrv = ir->data + end_intervals_pos;
    auto itrv_ovf = ir->data + ir->length_;  // TODO: create pre calculated  member

    if (k == 0)  //
        goto label;

    do {
        auto l_end = (g_end > step) ? step : g_end;

        while (k < l_end) {
            k++;
            addToPoint(*itrv, g_pos);
            INCREASE_G_POS();
        }

        g_end -= l_end;

        itrv++;
        itrv = itrv < itrv_ovf ? itrv : itrv - ir->length_;
    label:
        k = 1;
        initPoint(*itrv, g_pos);
        INCREASE_G_POS();
    } while (g_end);

    ir->buffer_pos_of_right_border_ = g_pos;

    ir->step_2_ = right_step;

    ir->start_pos_ += tmp / ir->step_;
    ir->start_pos_ = EXRT_remainder(ir->start_pos_, ir->length_);

    size_t time = buffer_time_step_ * number_of_points;
    ir->borders_ = ir->borders_ + (*(RelativeTime *)&time);
}

Borders PseudoSyncPrmBuffer::getAvailableBorders() {
    // TODO: this is the worst(incorrect) approach to synchronization
    while (updating_borders_)
        ;
    updating_borders_ = true;
    auto ret = Borders(buffer_start_time_, buffer_time_interval_);
    updating_borders_ = false;
    return ret;
}

double PseudoSyncPrmBuffer::getIntervalsBufferStep(IntervalBuffer *current) {  //
    return (double)current->length_ / ((double)(current->borders_.getInterval()).time);
}

bool PseudoSyncPrmBuffer::lock(bool arg) {
    if (set_data_in_progress && arg) return false;
    is_locked = arg;
    return true;
}

bool PseudoSyncPrmBuffer::isLock() { return is_locked; }
