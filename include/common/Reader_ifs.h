#ifndef EXRT_READER_IFS_H
#define EXRT_READER_IFS_H
#include <cstdint>
#include <string>

#include "TimeRepresentation.h"
#include "common\Port.h"

class Reader_ifs {
   public:
    virtual ~Reader_ifs() {}

    typedef double ReaderData;

    struct Borders {
        inline Borders(const AbsoluteTime& a, const AbsoluteTime& b) {
            begin = a;
            end = b;
        }
        explicit inline Borders(const AbsoluteTime& a, const RelativeTime& b) {
            begin = a;
            end = a + b;
        }

        AbsoluteTime begin;
        AbsoluteTime end;
        bool belongsTo(const Borders& cm) const { return (cm.begin <= begin) && (end <= cm.end); }
        RelativeTime getInterval() const { return end - begin; }
        Borders operator+(const RelativeTime& time_interval) const {
            return Borders(begin + time_interval, end + time_interval);
        }
    };

    struct Point {
        RelativeTime pos;
        ReaderData sum;
        ReaderData max;
        ReaderData min;
    };

    class Inerval {
       public:
        Inerval() : borders_(AbsoluteTime(), AbsoluteTime()){};
        Inerval(const Inerval& prototype) = delete;

        ~Inerval() {
            while (Inerval* current = next_) {
                next_ = next_->next_;
                current->next_ = nullptr;
                delete current;
            }
        }

        Borders borders_;

        size_t number_of_points_;
        Point* first_point_;

        std::string error_mesadge_;

        Inerval* next_ = nullptr;
    };

    virtual bool lock(bool arg) = 0;
    virtual bool isLock() = 0;

    virtual Inerval getPoints(const Borders& begin, Point* ptr, size_t point_count) = 0;

    virtual Borders getAvailableBorders() = 0;
};

#endif