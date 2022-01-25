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
    succes = true,
    failure = false
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
typedef char *str_v_t;

union ArbitraryData {
    ArbitraryData() { none_v = nullptr; }

    void *none_v;

    u8_t u8{};
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

#define COMMON_SINT 0x100  // all int and uint
#define COMMON_UINT 0x200  // signed int
#define COMMON_INT 0x300   // unsigned int
#define COMMON_NUM 0x700   // all int, uint and float

#define COMMON_FlOAT 0x400  // unsigned int
#define COMMON_STR 0x3000
#define COMMON_PTR 0x3000

inline size_t getTSize(DataType type) { return (size_t)0xf & (size_t)type; }

inline bool isNum(DataType type) { return (bool)(COMMON_NUM & (size_t)type); }

inline bool isInt(DataType type) { return (bool)(COMMON_INT & (size_t)type); }

inline bool isSInt(DataType type) { return (bool)(COMMON_SINT & (size_t)type); }

inline bool isUnt(DataType type) { return (bool)(COMMON_UINT & (size_t)type); }

inline bool isFloat(DataType type) { return (bool)(COMMON_FlOAT & (size_t)type); }

inline bool isBool(DataType type) { return (bool)(DataType::bool_v == type); }

inline bool isString(DataType type) { return (bool)(COMMON_STR & (size_t)type); }

inline bool isPtr(DataType type) { return (bool)(COMMON_PTR & (size_t)type); }

inline std::string getString(str_v_t data) {  //
    return std::string(data + sizeof(size_t), *((size_t *)data));
}

inline str_v_t createStrV(const std::string &str) {  //
    size_t size = str.size();
    auto data = new char[size + sizeof(size_t)];
    *((size_t *)data) = size;
    memcpy((void *)(data + sizeof(size_t)), str.c_str(), size);
    return data;
}

inline size_t getSize(const ArbitraryData data, DataType type) {
    return isPtr(type) ? *((size_t *)data.str_v) : getTSize(type);
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
    case lstr_v:
        return getString(data.str_v);

    case none_v:
    default:
        break;
    }
    return "";
#undef TO_STR
}

COMMON_API_ DataType createDataType(const std::string &str);

COMMON_API_ std::string toString(DataType type);

inline ArbitraryData createArbitraryData(const std::string &str, DataType type, bool &status) {
    ArbitraryData res;
    status = true;
    res.u64 = 0;
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
        res.bool_v = (str == "true");

        break;
    case str_v:
    case lstr_v:
        res.str_v = createStrV(str);
        break;

    case none_v:
    default:
        status = false;
        break;
    }

    return res;
}

inline ArbitraryData copyArbitraryData(const ArbitraryData &data, DataType type) {
    ArbitraryData res = data;
    if (isPtr(type)) {
        size_t size = (*((size_t *)data.str_v)) + sizeof(size_t);
        res.str_v = new char[size];
        memcpy(res.str_v, data.str_v, size);
    }
    return res;
}

inline ArbitraryData copyArbitraryData(const void *ptr, DataType type) {
    ArbitraryData res = ArbitraryData();
    if (!isPtr(type)) memcpy(&res.u64, ptr, getTSize(type));
    return res;
}

struct Value {
    Value() : type_(DataType::none_v) {}

    Value(const Value &value) : value_(copyArbitraryData(value.value_, value.type_)), type_(value.type_) {}

    Value &operator=(const Value &value) {
        if (this == &value) return *this;

        value_ = copyArbitraryData(value.value_, value.type_);
        type_ = value.type_;
        return *this;
    }

    Value(const Value &arg, DataType target_type) : type_(DataType::none_v) {
        if (arg.type_ == type_) {
            value_ = copyArbitraryData(arg.value_, arg.type_);
            type_ = arg.type_;
        } else if (isInt(arg.type_)) {
            if (!isInt(target_type)) return;
        } else if (isString(arg.type_)) {
            if (!isString(target_type)) return;
            type_ = target_type;
            value_ = copyArbitraryData(arg.value_, arg.type_);
        } else if (isFloat(arg.type_)) {
            if (!isFloat(target_type)) return;
            type_ = target_type;
            if (arg.type_ == DataType::f64) value_.f32 = (float)arg.value_.f64;
            else if (arg.type_ == DataType::f32)
                value_.f64 = (double)arg.value_.f32;
        } else if (isBool(arg.type_)) {
            if (!isString(target_type)) return;
            type_ = target_type;
            value_ = copyArbitraryData(arg.value_, arg.type_);
        }
    }

    explicit Value(const char *a) : type_(DataType::str_v) { value_.str_v = createStrV(a); }

    explicit Value(const std::string &a) : type_(DataType::str_v) { value_.str_v = createStrV(a); }

    explicit Value(bool a) : type_(DataType::bool_v) { value_.bool_v = a; }

    explicit Value(double a) : type_(DataType::f64) { value_.f64 = a; }

    explicit Value(int64_t a) : type_(DataType::i64) { value_.i64 = a; }

    Value(const void *ptr, DataType type) : value_(copyArbitraryData(ptr, type)), type_(type) {}

    Value(const std::string &str, DataType type) : type_(DataType::none_v) {
        bool status = false;
        value_ = createArbitraryData(str, type, status);
        if (status) type_ = type;
    }

    Value(const std::string &str, const std::string &str_type) : type_(DataType::none_v) {
        auto type = createDataType(str_type);

        bool status = false;
        value_ = createArbitraryData(str, type, status);
        if (status) type_ = type;
    }

    explicit operator bool() const { return type_ != none_v; }

    bool operator!() const { return type_ == none_v; }

    std::string asString() { return ::asString(value_, type_); }

    ~Value() {
        // std::cout << "del Value\n";
        if (isPtr(type_)) delete[] value_.str_v;
    }

    ArbitraryData value_;
    DataType type_;
};

inline std::string toString(const Value &value) { return toString(value); }

struct EthernetAddress {
    uint8_t mac[6];
    uint8_t port[2];
    uint8_t ip_b[4];
};

struct EthernetSettings {
    EthernetSettings() { memset(this, 0, sizeof(EthernetSettings)); }
    EthernetSettings(const EthernetAddress &dst, const EthernetAddress &src) : src_(src), dst_(dst) {}

    EthernetAddress dst_;
    EthernetAddress src_;
};

class COMMON_API_ HierarchicalData_ifs {
   public:
    typedef std::vector<std::pair<std::string, HierarchicalData_ifs *>> getMapReturn_t;

    virtual ~HierarchicalData_ifs() = default;

    [[nodiscard]] virtual bool isArray() const = 0;

    [[nodiscard]] virtual bool isMap() const = 0;

    [[nodiscard]] virtual bool isValue() const = 0;

    [[nodiscard]] virtual Value getValue() const = 0;

    [[nodiscard]] virtual std::vector<HierarchicalData_ifs *> getArray() const = 0;

    [[nodiscard]] virtual getMapReturn_t getMap() const = 0;

    [[nodiscard]] virtual HierarchicalData_ifs *getArrayUnit(size_t index) const = 0;

    /*  need provide access not only to map entries(units) also to array entries(units);
     *  getMapUnit("10") is expected to behave like getArrayUnit(10). */
    [[nodiscard]] virtual HierarchicalData_ifs *getMapUnit(std::string field_name) const = 0;
};

COMMON_API_ const HierarchicalData_ifs *getBranch(const HierarchicalData_ifs *h_data, const std::string path);

COMMON_API_ std::string toString(const HierarchicalData_ifs *h_data, const std::string &indent);

#endif