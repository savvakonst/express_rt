#ifndef EXRT_TIMEREPRESENTATION_H
#define EXRT_TIMEREPRESENTATION_H

#include <cstdint>

struct RelativeTime {
    union {
        struct {
            uint32_t ms_fractional;
            int32_t ls_integer;
        };
        int64_t time;
    };
    double toDouble() const {
        double d = ls_integer;
        d  += (static_cast<double>(ms_fractional) / (1 << 31));
        return d;
    }
    void fromDouble(const double val){
        time = static_cast<int64_t>(val * (1ull << 31));
    }
};

/*inline RelativeTimeFrom(double arg){
    RelativeTime t;

    return (t)
}*/


struct AbsoluteTime {
    explicit AbsoluteTime() : long_fractional(0), unix_time(0) {}
    union {
        struct {
            uint32_t ls_fractional;
            uint32_t ms_fractional;
        };
        uint64_t long_fractional;
    };
    int64_t unix_time;
};

inline bool operator==(const AbsoluteTime& left, const AbsoluteTime& right) {
    return (left.unix_time == right.unix_time) && (left.long_fractional == right.long_fractional);
}

inline bool operator<(const AbsoluteTime& left, const AbsoluteTime& right) {
    if (left.unix_time < right.unix_time) return true;
    return (left.unix_time == right.unix_time) && (left.long_fractional < right.long_fractional);
}

inline bool operator>(const AbsoluteTime& left, const AbsoluteTime& right) {
    if (left.unix_time > right.unix_time) return true;
    return (left.unix_time == right.unix_time) && (left.long_fractional > right.long_fractional);
}

inline bool operator<=(const AbsoluteTime& left, const AbsoluteTime& right) {
    if (left.unix_time < right.unix_time) return true;
    return (left.unix_time == right.unix_time) && (left.long_fractional <= right.long_fractional);
}

inline bool operator>=(const AbsoluteTime& left, const AbsoluteTime& right) {
    if (left.unix_time > right.unix_time) return true;
    return (left.unix_time == right.unix_time) && (left.long_fractional >= right.long_fractional);
}

inline bool operator==(const RelativeTime& left, const RelativeTime& right) { return (left.time == right.time); }

inline bool operator<(const RelativeTime& left, const RelativeTime& right) {
    if (left.ls_integer < right.ls_integer) return true;
    return (left.ls_integer == right.ls_integer) && (left.ms_fractional < right.ms_fractional);
}

inline bool operator>(const RelativeTime& left, const RelativeTime& right) {
    if (left.ls_integer > right.ls_integer) return true;
    return (left.ls_integer == right.ls_integer) && (left.ms_fractional > right.ms_fractional);
}


inline bool operator>=(const RelativeTime& left, const RelativeTime& right) {
    if (left.ls_integer > right.ls_integer) return true;
    return (left.ls_integer == right.ls_integer) && (left.ms_fractional >= right.ms_fractional);
}

inline RelativeTime operator-(const AbsoluteTime& left, const AbsoluteTime& right) {
    RelativeTime ret = {0};

    ret.ms_fractional = left.ms_fractional - right.ms_fractional;
    ret.ls_integer =
        int32_t(left.unix_time) - int32_t(right.unix_time) - (left.long_fractional < right.long_fractional);  // unsafe
    // ret.ls_ineger -= (left.long_fractional < right.long_fractional) ? 1 : 0;
    return ret;
}

inline AbsoluteTime operator+(const AbsoluteTime& left, const RelativeTime& right) {
    AbsoluteTime ret;
    RelativeTime intermediate;
    intermediate.time = ((int64_t)left.ms_fractional) + ((int64_t)right.ms_fractional);

    ret.ms_fractional = intermediate.ms_fractional;
    ret.unix_time = left.unix_time + (int64_t)right.ls_integer + (int64_t)intermediate.ls_integer;
    return ret;
}

inline AbsoluteTime operator-(const AbsoluteTime& left, const RelativeTime& right) {
    AbsoluteTime ret;
    RelativeTime intermediate;
    intermediate.time = ((int64_t)left.ms_fractional) - ((int64_t)right.ms_fractional);

    ret.ms_fractional = intermediate.ms_fractional;
    ret.unix_time = left.unix_time - (int64_t)right.ls_integer + (int64_t)intermediate.ls_integer;
    return ret;
}

inline RelativeTime operator+(const RelativeTime& left, const RelativeTime& right) {
    RelativeTime ret;
    ret.time = left.time + right.time;
    return ret;
}

// TODO: Check result
inline RelativeTime operator+=(const RelativeTime& left, const RelativeTime& right) {
    RelativeTime ret;
    ret.time = left.time + right.time;
    return ret;
}

inline RelativeTime operator-(const RelativeTime& left, const RelativeTime& right) {
    RelativeTime ret;
    ret.time = left.time - right.time;
    return ret;
}

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

    inline bool belongsTo(const Borders& cm) const { return (cm.begin <= begin) && (end <= cm.end); }
    inline RelativeTime getInterval() const { return end - begin; }
    inline Borders operator+(const RelativeTime& time_interval) const {
        return Borders(begin + time_interval, end + time_interval);
    }
};

#endif
