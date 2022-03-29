#ifndef EXRT_READER_IFS_H
#define EXRT_READER_IFS_H
#include <cstdint>
#include <memory>
#include <string>

#include "TimeRepresentation.h"
#include "common\Port.h"

class Parameter_ifs;

class Reader_ifs {
   public:
    virtual ~Reader_ifs() = default;

    typedef double readerData_t;

    struct Point {
        uint64_t pos;
        readerData_t sum;
        readerData_t max;
        readerData_t min;
    };

    class Chunk {
       public:
        enum Status : size_t
        {
            success = 0,
            out_of_borders = 1,
            no_valid_data = 2,
            no_connection = 3,
        };

        inline Chunk()  //
            : borders_(AbsoluteTime(), AbsoluteTime()){};

        inline Chunk(const Borders& borders, Point* first_point, size_t number_of_points)  //
            : borders_(borders), first_point_(first_point), number_of_points_(number_of_points) {}

        // Chunk(const Chunk& prototype) = delete;

        ~Chunk() {
            while (Chunk* current = next_) {
                next_ = next_->next_;
                current->next_ = nullptr;
                delete current;
            }
        }

        Borders borders_;

        Point* first_point_ = nullptr;
        size_t number_of_points_ = 0;

        Chunk::Status error_code_ = Status::success;

        Chunk* next_ = nullptr;
    };

    virtual Parameter_ifs* getParameter() { return nullptr; }

    virtual bool lock(bool arg) = 0;
    virtual bool isLock() = 0;

    virtual std::unique_ptr<Chunk> getPoints(const Borders& begin, Point* ptr, size_t point_count) = 0;

    virtual Borders getAvailableBorders() = 0;
};

#endif