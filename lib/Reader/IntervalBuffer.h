//
// Created by SVK on 22.11.2021.
//

#ifndef EXRT_INTERVALBUFFER_H
#define EXRT_INTERVALBUFFER_H

#include <memory>

#include "common/Reader_ifs.h"
#define EXRT_remainder(X, Y) (((X) < (Y)) ? (X) : (X - Y))

class PseudoSyncPrmBuffer;

class IntervalBuffer : public Reader_ifs {
   public:
    IntervalBuffer(PseudoSyncPrmBuffer *parent, const Borders &borders) : parent_(parent), borders_(borders){};
    ~IntervalBuffer() override { delete[] data; }

    std::unique_ptr<Reader_ifs::Chunk> getPoints(const Borders &borders, Point *ptr, size_t target_len) override;
    Borders getAvailableBorders();

    bool lock(bool arg) override;
    bool isLock() override;

    Reader_ifs::Point *data = nullptr;

    size_t seed_length_ = 0;

    /*    this is the actual value of intervals buffer length;
     * it is more or equal than seed_intervals_length_ but less
     * than (seed_intervals_length_ * 2 + 2) */
    size_t length_ = 0;

    /*    this is the pos in intervals buffer from witch starts intervals counting
     * (pos of first interval in in buffer ) */
    size_t start_pos_ = 0;

    Point *itrv_ = 0;

    /*    this is the number of points which are contained
     * in "intervals_borders_" */
    size_t capacity_ = 0;

    Borders borders_;  // = {AbsoluteTime(), AbsoluteTime()};

    size_t ord_ = 0;
    size_t step_ = 0;
    size_t step_1_ = 0;
    size_t step_2_ = 0;

    size_t buffer_pos_of_right_border_ = 0;

    PseudoSyncPrmBuffer *parent_;
};

#endif  // HELLOWORLD_INTERVALBUFFER_H
