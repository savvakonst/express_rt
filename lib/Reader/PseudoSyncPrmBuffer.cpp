//
// Created by SVK on 01.11.2021.
//
#include <cstring>
//
#include "IntervalBuffer.h"
#include "PseudoSyncPrmBuffer.h"

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
    size_t sample_frequency_log_2,         //
    bufferType_t *init_buffer,             //
    size_t intervals_buffer_length,        //
    RelativeTime buffer_interval)
    : sample_frequency_log_2_(sample_frequency_log_2),
      seed_intervals_length_(intervals_buffer_length),
      buffer_time_interval_(buffer_interval),
      buffer_length_(uint64_t(uint64_t(buffer_interval.time) * uint64_t(uint64_t(1) << sample_frequency_log_2_)) >> 32),
      buffer_time_step_(uint64_t(1) << uint64_t(32 - sample_frequency_log_2_)),
      PrmBuffer_ifs(parent)  //
{
    buffer_ = new bufferType_t[buffer_length_];
    buffer_start_pos_ = 0;

    { std::memset(buffer_, 0, buffer_length_ * sizeof(bufferType_t)); }

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

Reader_ifs *PseudoSyncPrmBuffer::createReader() {
    auto *ir = new IntervalBuffer(this, Borders(buffer_start_time_, buffer_time_interval_));

    ir->seed_length_ = seed_intervals_length_;
    ir->borders_ = Borders(buffer_start_time_, buffer_time_interval_);

    auto size = 2 * ir->seed_length_ + 2;
    ir->data_ = new Reader_ifs::Point[2 * ir->seed_length_ + 2];

    for (size_t i = 0; i < size; i++) ir->data_[i] = {0, 0, 0, 0};

    ir->capacity_ = getIntervalCapacity(ir->borders_);
    ir->step_ = (ir->capacity_ / ir->seed_length_);
    if (ir->capacity_<ir->seed_length_)



    ir->start_pos_ = ir->capacity_ / ir->step_ + 2;
    ir->remainder_ = ir->capacity_ % ir->step_;
    ir->length_ = ir->capacity_ / ir->step_ + 2;
    ir->data_ovf_ = ir->data_ + ir->length_;

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

    // TODO: maybe it should be "ir->pos_of_right_buffer_border_ = buffer_start_pos_ - 1 or + 1 ;"
    ir->pos_of_right_buffer_border_ = buffer_start_pos_;
    increasePoints(buffer_length_, ir);
    return ir;
}

void PseudoSyncPrmBuffer::setData(char *data, size_t length, DataStatus status) {
    set_data_in_progress_ = true;
    while (is_locked_) {
    }

    update(data, length);

    auto ir_buffers = ir_buffers_;

    while (*ir_buffers) increasePoints(length, *(ir_buffers++));

    set_data_in_progress_ = false;
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

    bufferType_t *typed_data = (bufferType_t *)data;

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
    ir->buffer_pos_of_right_border_ = EXRT_remainder(ir->pos_of_right_buffer_border_, buffer_length_);

#include <iostream>
void PseudoSyncPrmBuffer::increasePoints(size_t number_of_points, IntervalBuffer *ir) {
    if (number_of_points >= ir->capacity_) {
        // FIXME: be attentive
        std::cout << "ir->pos_of_right_buffer_border_: " << ir->pos_of_right_buffer_border_;
        std::cout << ", ir->number_of_points: " << number_of_points << "\n";

        ir->pos_of_right_buffer_border_ += (number_of_points - ir->capacity_);
        ir->pos_of_right_buffer_border_ = ir->pos_of_right_buffer_border_ < buffer_length_
                                              ? ir->pos_of_right_buffer_border_
                                              : ir->pos_of_right_buffer_border_ % buffer_length_;

        ir->start_pos_ = 0;
        ir->k_r_ = ir->remainder_;

        // FIXME: Remove this dummy and determine where the overflow occurs.
        ir->pos_of_right_buffer_border_ = EXRT_remainder(ir->pos_of_right_buffer_border_, buffer_length_);
        size_t last_index = ir->pos_of_right_buffer_border_;

        for (size_t i = 0; i < ir->length_ - 2; i++) {
            last_index = EXRT_remainder(last_index, buffer_length_);
            auto next_index = last_index + ir->step_;

            auto &interval = ir->data_[i];

            initPoint(interval, last_index);
            for (size_t j = last_index + 1; (j < next_index); j++) {
                addToPoint(interval, EXRT_remainder(j, buffer_length_));
            }

            // interval.pos.ms_fractional = uint32_t(ir->step_);
            last_index = next_index;
        }

        auto next_index = last_index + ir->remainder_;
        auto &interval = ir->data_[ir->length_ - 1];
        initPoint(interval, EXRT_remainder(last_index, buffer_length_));
        for (size_t j = last_index + 1; (j < next_index); j++) {
            addToPoint(interval, EXRT_remainder(j, buffer_length_));
        }

        ir->pos_of_right_buffer_border_ = EXRT_remainder(next_index, buffer_length_);

        // bool is_additional_sell = ir->step_2_ >= ir->ord_;
        // ir->itrv_ = ir->data_ + ((ir->start_pos_ + ir->length_ - 2) % ir->length_);

#ifdef DEBUG_
        std::cout << "end ir->pos_of_right_buffer_border_: " << ir->pos_of_right_buffer_border_;
        std::cout << ", ir->number_of_points: " << number_of_points << "\n";
#endif
        return;
    }
    /*
        // begin calculation
        auto tmp = number_of_points - ir->step_1_ + ir->step_;
        ir->step_1_ = ir->step_ - ((number_of_points < ir->step_1_) ? number_of_points : tmp % ir->step_);

        // end calculation
        bool is_additional_sell = ir->step_2_ >= ir->ord_;
        // TODO: CREATE MEMBER end_pos_ for IntervalBuffer
        auto end_intervals_pos = (ir->start_pos_ + ir->length_ - 1 - size_t(is_additional_sell)) % ir->length_;

        tmp = number_of_points + ir->step_2_;
        auto right_step = tmp % ir->step_;

        */
    auto g_pos = ir->pos_of_right_buffer_border_;

#define INCREASE_G_POS() \
    g_pos++;             \
    g_pos = EXRT_remainder(g_pos, buffer_length_)

    auto step = ir->step_;
    auto cnt = ir->k_r_;
    auto g_end = cnt + number_of_points;

    auto itrv = ir->data_ + ir->start_pos_;
    auto itrv_ovf = ir->data_ovf_;

    if ((cnt != 0) && (g_end >= step))  //
        goto label;

    while (g_end >= step) {
        initPoint(*itrv, g_pos);
        INCREASE_G_POS();
        cnt = 1;
    label:

        while (cnt < step) {
            addToPoint(*itrv, g_pos);
            INCREASE_G_POS();
            cnt++;
        }
        g_end -= step;

        itrv++;
        itrv = itrv < itrv_ovf ? itrv : itrv - ir->length_;
    }

    if (cnt == step) cnt = 0;

    if (g_end) {
        if (cnt == 0) {
            initPoint(*itrv, g_pos);
            INCREASE_G_POS();
            cnt++;
        }
        while (cnt < g_end) {
            cnt++;
            addToPoint(*itrv, g_pos);
            INCREASE_G_POS();
        }
    }

    ir->k_r_ = g_end;
    ir->pos_of_right_buffer_border_ = g_pos;

    ir->start_pos_ = itrv - ir->data_;

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
    if (set_data_in_progress_ && arg) return false;
    is_locked_ = arg;
    return true;
}

bool PseudoSyncPrmBuffer::isLock() { return is_locked_; }
