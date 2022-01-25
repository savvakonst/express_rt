//
// Created by SVK on 22.11.2021.
//

#include "IntervalBuffer.h"

#include <iostream>

#include "PseudoSyncPrmBuffer.h"

bool IntervalBuffer::lock(bool arg) { return parent_->lock(arg); }

bool IntervalBuffer::isLock() { return parent_->is_locked; }

Borders IntervalBuffer::getAvailableBorders() {
    // TODO  I'm not sure what is worth returning these boundaries. may be parent->borders_
    return parent_->getAvailableBorders();
};

std::unique_ptr<Reader_ifs::Chunk> IntervalBuffer::getPoints(const Borders &borders, Reader_ifs::Point *ptr,
                                                             size_t target_len) {

    while (false == parent_->lock(true)) {
    }
    Borders cr_borders = borders;

    std::unique_ptr<Chunk> ret(new Chunk());
    Chunk *chunk_ptr = ret.get();
    Chunk *end_chunk = nullptr;

    auto time_interval = borders.getInterval().time;

    auto buff_step = parent_->getIntervalsBufferStep(this);
    auto target_step = double(target_len) / double(time_interval);

    size_t pos_offset = 0;

    if (cr_borders.begin < parent_->buffer_start_time_) {
        chunk_ptr->next_ = new Chunk();
        chunk_ptr->borders_ = {cr_borders.begin, parent_->buffer_start_time_};
        chunk_ptr->error_code_ = Chunk::Status::out_of_borders;

        cr_borders.begin = parent_->buffer_start_time_;
        pos_offset = size_t(target_step * double(chunk_ptr->borders_.getInterval().time));
        target_len -= pos_offset;

        chunk_ptr = chunk_ptr->next_;
    }

    auto parent_end_time = (parent_->buffer_start_time_ + parent_->buffer_time_interval_);
    if (cr_borders.end > parent_end_time) {
        end_chunk = new Chunk();
        end_chunk->borders_ = {parent_end_time, cr_borders.end};
        end_chunk->error_code_ = Chunk::Status::out_of_borders;

        cr_borders.end = parent_end_time;

        target_len -= size_t(target_step * double(end_chunk->borders_.getInterval().time));
    }

    // TODO create intermediate chunk

    if (!(cr_borders.belongsTo(borders_) && (buff_step >= target_step))) {
        size_t begin = parent_->getStartIndex(cr_borders);
        size_t end = parent_->getEndIndex(cr_borders);
        size_t cap = end - begin;

        if (cap == 0) return {};
        if (cap < target_len) {
            /* TODO: to implement direct reading mode */
            return {};
        }
        capacity_ = cap;

        borders_ = cr_borders;

        step_ = (capacity_ / seed_length_);
        length_ = capacity_ / step_ + 2;
        ord_ = capacity_ % step_;

        auto tmp = parent_->buffer_start_pos_ + begin;
        buffer_pos_of_right_border_ = EXRT_remainder(tmp, parent_->buffer_length_);
        parent_->increasePoints(capacity_, this);
    }

    auto step = buff_step / target_step;

    size_t begin_offset = start_pos_ + size_t(buff_step * double((cr_borders.begin - borders_.begin).time));
    size_t first_end_offset = begin_offset + size_t(step * target_len);

    size_t second_end_offset = 0;

    if (first_end_offset > length_) {
        second_end_offset = first_end_offset - length_;
        first_end_offset = length_;
    }

    size_t last_index = begin_offset;

    Point *il = ptr - 1;
    Point &buf_interval = data[last_index];

    size_t pos = 0;
    size_t transition_index = begin_offset;

    for (size_t i = begin_offset; i < first_end_offset; i++) {
        Point &val = data[i];
        if (i == transition_index) {
            il++;
            pos++;

            transition_index = begin_offset + size_t(double(pos) * step);

            *il = val;
            il->pos = pos_offset;
            pos_offset++;
        } else {
            il->sum += val.sum;
            il->max = std::max(il->max, val.max);
            il->min = std::min(il->min, val.min);
        }
    }

    transition_index = EXRT_remainder(transition_index, length_);
    begin_offset = length_ - begin_offset;

    for (size_t i = 0; i < second_end_offset; i++) {
        Point &val = data[i];
        if (i == transition_index) {
            il++;
            pos++;
            transition_index = size_t(double(pos) * step) - begin_offset;

            *il = val;
            il->pos = pos_offset;
            pos_offset++;
        } else {
            il->sum += val.sum;
            il->max = std::max(il->max, val.max);
            il->min = std::min(il->min, val.min);
        }
    }

    parent_->lock(false);
    /*
    if ((ptr + target_len - 1) != (il)) {
        std::cout << "pos:        " << pos << "\n";
        std::cout << "target_len: " << target_len << "\n";
    }*/

    chunk_ptr->first_point_ = ptr;
    chunk_ptr->number_of_points_ = pos;
    chunk_ptr->borders_ = cr_borders;
    chunk_ptr->next_ = end_chunk;


    return ret;
}