#include "doctest.h"
//
#define UNIT_TEST
#include "Reader/IntervalBuffer.h"
#include "Reader/PseudoSyncPrmBuffer.h"

class UnitTestPseudoSyncPrmBufferFriend {
   public:
    size_t getBuffSize(PseudoSyncPrmBuffer* pbf) { return pbf->buffer_length_; }
    double* getBuffPtr(PseudoSyncPrmBuffer* pbf) { return pbf->buffer_; }
};

TEST_CASE("Check PseudoSyncPrmBuffer") {
    UnitTestPseudoSyncPrmBufferFriend ifs;

    size_t factor = 100;
    size_t short_size = 2;
    size_t long_size = short_size * factor;

    auto short_intervals = new PseudoSyncPrmBuffer(nullptr, 10);
    auto long_intervals = new PseudoSyncPrmBuffer(nullptr, 10);

    size_t buffer_length = ifs.getBuffSize(long_intervals);

    auto* short_buffer = new double[short_size];
    auto* long_buffer = new double[long_size];

    size_t short_cnt = 0;
    size_t long_cnt = 0;

    {
        for (auto i = 0; i < factor; i++) {
            for (auto pos = 0; pos < short_size; pos++) short_buffer[pos] = (double)(short_cnt++);
            short_intervals->setData((char*)short_buffer, short_size, DataStatus::success);
        }
        for (auto pos = 0; pos < long_size; pos++) long_buffer[pos] = (double)(long_cnt++);
        long_intervals->setData((char*)long_buffer, long_size, DataStatus::success);
    }

    auto short_reader = (IntervalBuffer*)short_intervals->createIntervalBuffer();
    auto long_reader = (IntervalBuffer*)long_intervals->createIntervalBuffer();

    {
        bool reader_buffers_are_similar = true;
        int err_cnt = 0;
        for (size_t pos = 0; pos < long_reader->length_; pos++) {
            bool c = (long_reader->data_[pos].max == short_reader->data_[pos].max);
            if (!c) err_cnt++;
            reader_buffers_are_similar = reader_buffers_are_similar && c;
        }

        {
            INFO("incorrect initialization of reader buffers. \n buffer step equal  ", long_reader->step_, " \n ",
                 err_cnt, " mistakes from ", long_reader->length_, " cases");
            CHECK(reader_buffers_are_similar == true);
            CHECK(long_reader->start_pos_ == short_reader->start_pos_);
            CHECK(long_reader->k_r_ == short_reader->k_r_);
            CHECK(long_reader->pos_of_right_buffer_border_ == short_reader->pos_of_right_buffer_border_);
            CHECK(long_reader->borders_.end == short_reader->borders_.end);
            CHECK(long_reader->borders_.begin == short_reader->borders_.begin);
        }
    }

    double* short_intervals_buffer = ifs.getBuffPtr(short_intervals);
    double* long_intervals_buffer = ifs.getBuffPtr(long_intervals);

    size_t max_iter_cnt = buffer_length / (long_size - 1);

    for (size_t iter = 0; iter < max_iter_cnt * 2; iter++) {
        for (auto i = 0; i < factor; i++) {
            for (auto pos = 0; pos < short_size; pos++) short_buffer[pos] = (double)(short_cnt++);
            short_intervals->setData((char*)short_buffer, short_size, DataStatus::success);
        }

        for (auto pos = 0; pos < long_size; pos++) long_buffer[pos] = (double)(long_cnt++);
        long_intervals->setData((char*)long_buffer, long_size, DataStatus::success);

        bool arrays_are_similar = true;
        for (size_t pos = 0; pos < buffer_length; pos++) {
            arrays_are_similar = arrays_are_similar && (long_intervals_buffer[pos] == short_intervals_buffer[pos]);
        }

        {
            INFO("incorrect prm buffers on ", iter, " iteration from ", max_iter_cnt, " iterations");
            CHECK(arrays_are_similar == true);
        }

        bool reader_buffers_are_similar = true;
        int err_cnt = 0;
        for (size_t pos = 0; pos < long_reader->length_; pos++) {
            bool c = (long_reader->data_[pos].sum == short_reader->data_[pos].sum);
            if (!c) err_cnt++;
            reader_buffers_are_similar = reader_buffers_are_similar && c;
        }

        {
            INFO("incorrect reader buffers on ", iter, " iteration from ", max_iter_cnt,
                 " iterations.\n buffer step equal ", long_reader->step_, ".\n ", err_cnt, " mistakes from ",
                 long_reader->length_, " cases.");
            CHECK(reader_buffers_are_similar == true);
            CHECK(long_reader->start_pos_ == short_reader->start_pos_);
            CHECK(long_reader->k_r_ == short_reader->k_r_);
            CHECK(long_reader->pos_of_right_buffer_border_ == short_reader->pos_of_right_buffer_border_);
            CHECK(long_reader->borders_.end == short_reader->borders_.end);
            CHECK(long_reader->borders_.begin == short_reader->borders_.begin);

            CHECK(long_reader->data_ovf_ == (long_reader->data_ + long_reader->length_));
            CHECK(short_reader->data_ovf_ == (short_reader->data_ + short_reader->length_));
        }
        if (!reader_buffers_are_similar) break;
    }
}
