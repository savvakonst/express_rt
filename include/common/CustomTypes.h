#ifndef EXRT_CUSTOMTYPES_H
#define EXRT_CUSTOMTYPES_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "common/Port.h"

#define exo_container std::list
#define exo_map std::map
#define exo_set std::set

enum status : bool
{
    succes = false,
    failure = true
};

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint64_t u64_t;

typedef int8_t i8_t;
typedef int16_t i16_t;
typedef int32_t i32_t;
typedef int64_t i64_t;

typedef float f32_t;
typedef double f64_t;

typedef bool bool_v_t;
typedef char* str_v_t;

union ArbitraryData {
    ArbitraryData() { none_v = nullptr; }

    void* none_v;

    u8_t u8;
    u16_t u16;
    u32_t u32;
    u64_t u64;

    i8_t i8;
    i16_t i16;
    i32_t i32;
    i64_t i64;

    float f32;
    double f64;

    bool_v_t bool_v;
    str_v_t str_v;
};

enum DataType : size_t
{
    none_v = 0x00,

    i8 = 0x101,
    i16 = 0x102,
    i32 = 0x104,
    i64 = 0x108,

    u8 = 0x201,
    u16 = 0x202,
    u32 = 0x204,
    u64 = 0x208,

    f32 = 0x404,
    f64 = 0x408,

    bool_v = 0x801,
    str_v = 0x1000,
    lstr_v = 0x2000,
};

#define MAP_DEF                                                                                                 \
    PAIR(i8), PAIR(i16), PAIR(i32), PAIR(i64), PAIR(u8), PAIR(u16), PAIR(u32), PAIR(u64), PAIR(f32), PAIR(f64), \
        PAIR(bool_v), PAIR(str_v), PAIR(lstr_v)

#define PAIR(X) \
    { #X, X }
static const std::map<std::string, DataType> g_str_types_map = {MAP_DEF};
#undef PAIR
#define PAIR(X) \
    { X, #X }
static const std::map<DataType, std::string> g_types_str_map = {MAP_DEF};
#undef PAIR

#define COMMON_SINT 0x100  // all int and uint
#define COMMON_UINT 0x200  // signed int
#define COMMON_INT 0x300   // unsigned int
#define COMMON_NUM 0x700   // all int, uint and float

#define COMMON_FlOAT 0x400  // unsigned int
#define COMMON_STR 0x3000
#define COMMON_PTR 0x3000

inline size_t getTSize(DataType type) { return (size_t)0xf & (size_t)type; }
inline bool isNum(DataType type) { return (bool)(COMMON_NUM & (size_t)type); }
inline bool isBool(DataType type) { return (bool)(DataType::bool_v == type); }
inline bool isString(DataType type) { return (bool)(COMMON_STR & (size_t)type); }
inline bool isPtr(DataType type) { return (bool)(COMMON_PTR & (size_t)type); }

inline std::string getString(str_v_t data) {  //
    return std::string(data + sizeof(size_t), *((size_t*)data));
}

inline str_v_t createStrV(const std::string& str) {  //
    size_t size = str.size();
    str_v_t data = new char[size + sizeof(size_t)];
    *((size_t*)data) = size;
    memcpy((void*)(data + sizeof(size_t)), str.c_str(), size);
    return data;
}

inline size_t getSize(const ArbitraryData data, DataType type) {
    return isPtr(type) ? *((size_t*)data.str_v) : getTSize(type);
}

inline std::string asString(const ArbitraryData data, DataType type) {
#define TO_STR(arg) \
case arg:           \
    return std::to_string(data.arg)

    switch (type) {
        TO_STR(i8);
        TO_STR(i16);
        TO_STR(i32);
        TO_STR(i64);
        TO_STR(u8);

        TO_STR(u16);
        TO_STR(u32);
        TO_STR(u64);

        TO_STR(f32);
        TO_STR(f64);

    case bool_v:
        return (data.bool_v) ? "true" : "false";
    case str_v:
        return getString(data.str_v);
    case lstr_v:
        return getString(data.str_v);

    case none_v:
        break;
    default:
        break;
    }
    return "";
#undef TO_STR
}

inline DataType createDataType(const std::string& str) {
    auto val = g_str_types_map.find(str);
    if (val != g_str_types_map.end()) return val->second;

    if (str == "string") return DataType::str_v;
    else if (str == "long_string")
        return DataType::lstr_v;
    else if (str == "bool")
        return DataType::bool_v;
    else if (str == "int")
        return DataType::i32;

    return DataType::none_v;
}

inline std::string toString(DataType type) {
    auto val = g_types_str_map.find(type);
    if (val != g_types_str_map.end()) return val->second;
    return "none_v";
}

inline ArbitraryData createArbitraryData(const std::string& str, DataType type) {
    ArbitraryData res;
    res.i64 = 0;
    switch (type) {
    case i8:
        res.i8 = (i8_t)stoi(str);
        break;
    case i16:
        res.i16 = (i16_t)stoi(str);
        break;
    case i32:
        res.i32 = (i32_t)stoi(str);
        break;
    case i64:
        res.i64 = (i64_t)stoll(str);
        break;

    case u8:
        res.u8 = (u8_t)std::stoul(str);
        break;
    case u16:
        res.u16 = (u16_t)std::stoul(str);
        break;
    case u32:
        res.u32 = (u32_t)std::stoul(str);
        break;
    case u64:
        res.u64 = (u64_t)std::stoull(str);
        break;

    case f32:
        res.f32 = std::stof(str);
        break;
    case f64:
        res.f64 = std::stod(str);
        break;

    case bool_v:
        res.bool_v = (str == "true") ? true : false;
        break;
    case str_v:
        res.str_v = createStrV(str);
        break;
    case lstr_v:
        res.str_v = createStrV(str);
        break;

    case none_v:
        break;
    default:
        break;
    }

    return res;
}

inline ArbitraryData copyArbitraryData(const ArbitraryData& data, DataType type) {
    ArbitraryData res = data;
    if (isPtr(type)) {
        size_t size = *((size_t*)data.str_v);
        res.str_v = new char[size + sizeof(size_t)];
        memcpy(res.str_v, data.str_v, size);
    }
    return res;
}

inline ArbitraryData copyArbitraryData(const void* ptr, DataType type) {
    ArbitraryData res = ArbitraryData();
    if (!isPtr(type)) memcpy(&res.u64, ptr, getTSize(type));
    return res;
}

struct Value {
    Value() : type_(DataType::none_v) {}
    Value(const Value& value) : value_(copyArbitraryData(value.value_, value.type_)), type_(value.type_) {}
    Value(const void* ptr, DataType type) : value_(copyArbitraryData(ptr, type)), type_(type) {}
    Value(bool a) : type_(DataType::bool_v) { value_.bool_v = a; }
    Value(double a) : type_(DataType::f64) { value_.f64 = a; }
    Value(int64_t a) : type_(DataType::f64) { value_.i64 = a; }
    Value(const std::string& a) : type_(DataType::str_v) { value_.str_v = createStrV(a); }
    Value(const std::string& str, DataType type) : value_(createArbitraryData(str, type)), type_(type) {}

    ~Value() {
        if (isPtr(type_)) delete[] value_.str_v;
    }

    ArbitraryData value_;
    DataType type_;
};

class HierarchicalData_ifs;

class COMMON_API_ HierarchicalData_ifs {
   public:
    virtual bool isArray() const = 0;
    virtual bool isMap() const = 0;
    virtual bool isValue() const = 0;

    virtual Value getValue() const = 0;

    virtual std::vector<HierarchicalData_ifs*> getArray() const = 0;
    virtual std::map<std::string, HierarchicalData_ifs*> getMap() const = 0;

    virtual HierarchicalData_ifs* getArrayUint(size_t) const = 0;
    virtual HierarchicalData_ifs* getMapUint(std::string) const = 0;
};

#endif