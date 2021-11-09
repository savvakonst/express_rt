#ifndef EXRT_TIMEREPRESENTATION_H
#define EXRT_TIMEREPRESENTATION_H

#include <cstdint>

struct RelativeTime {
    union {
        struct {
            uint32_t ms_fractional;
            int32_t ls_ineger;
        };
        int64_t time;
    };
};

struct AbsoluteTime {
    explicit AbsoluteTime() : long_fractional(0), unix_time(0) {}
    union {
        struct {
            uint32_t ls_fractional;
            uint32_t ms_fractional;
        };
        int64_t long_fractional;
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

inline RelativeTime operator-(const AbsoluteTime& left, const AbsoluteTime& right) {
    RelativeTime ret = {0};

    ret.ms_fractional = left.ms_fractional - right.ms_fractional;
    ret.ls_ineger = int32_t(left.unix_time - right.unix_time);  // unsafe
    ret.ls_ineger += (left.long_fractional < right.long_fractional);
    return ret;
}

inline AbsoluteTime operator+(const AbsoluteTime& left, const RelativeTime& right) {
    AbsoluteTime ret;
    RelativeTime intermediate;
    intermediate.time = ((int64_t)left.ms_fractional) + ((int64_t)right.ms_fractional);

    ret.ms_fractional = intermediate.ms_fractional;
    ret.unix_time = left.unix_time + (int64_t)right.ls_ineger + (int64_t)intermediate.ls_ineger;
    return ret;
}

inline AbsoluteTime operator-(const AbsoluteTime& left, const RelativeTime& right) {
    AbsoluteTime ret;
    RelativeTime intermediate;
    intermediate.time = ((int64_t)left.ms_fractional) - ((int64_t)right.ms_fractional);

    ret.ms_fractional = intermediate.ms_fractional;
    ret.unix_time = left.unix_time - (int64_t)right.ls_ineger + (int64_t)intermediate.ls_ineger;
    return ret;
}

inline RelativeTime operator+(const RelativeTime& left, const RelativeTime& right) {
    RelativeTime ret;
    ret.time = left.time + right.time;
    return ret;
}

inline RelativeTime operator-(const RelativeTime& left, const RelativeTime& right) {
    RelativeTime ret;
    ret.time = left.time - right.time;
    return ret;
}

#endif
