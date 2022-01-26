#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-token-paste"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
//
#include "common/CustomTypes.h"

#define LIST(SRC)    \
    CASE_(i8, SRC);  \
    CASE_(i16, SRC); \
    CASE_(i32, SRC); \
    CASE_(i64, SRC); \
                     \
    CASE_(u8, SRC);  \
    CASE_(u16, SRC); \
    CASE_(u32, SRC); \
    CASE_(u64, SRC);

#define SWITCH(SRC, DST_COND) \
    switch (DST_COND) { LIST(SRC) }

#define TOP_SWITCH(DST_COND, SRC_COND) \
    switch (SRC_COND) {                \
        TOP_CASE_(i8, DST_COND);       \
        TOP_CASE_(i16, DST_COND);      \
        TOP_CASE_(i32, DST_COND);      \
        TOP_CASE_(i64, DST_COND);      \
                                       \
        TOP_CASE_(u8, DST_COND);       \
        TOP_CASE_(u16, DST_COND);      \
        TOP_CASE_(u32, DST_COND);      \
        TOP_CASE_(u64, DST_COND);      \
    }

#define CASE_(DST, SRC)                 \
    case DataType::##DST: {             \
        dst.##DST = (DST##_t)src.##SRC; \
        break;                          \
    }
#define TOP_CASE_(SRC, DST_COND) \
    case DataType::SRC: {        \
        SWITCH(SRC, DST_COND);   \
        break;                   \
    }

ArbitraryData cast(ArbitraryData src, DataType src_type, DataType dst_type) {
    ArbitraryData dst;
    TOP_SWITCH(dst_type, src_type);
    return dst;
}

#define CASE_(DST, SRC)                                         \
    {                                                           \
        ArbitraryData dst;                                      \
        dst = cast(SRC##.value_, SRC##.type_, DataType::##DST); \
        Value test_v(v, DataType::##DST);                       \
        CHECK(test_v.value_.##DST == dst.##DST);                \
    }

TEST_CASE("CheckConversion") {  // 12/2/2020 -> 737761

    SUBCASE("-1256, DataType::i32;") {
        Value v(std::string("-1256"), DataType::i32);
        LIST(v);
    }
    SUBCASE(" 1256, DataType::i32") {
        Value v(std::string("1256"), DataType::i32);
        LIST(v);
    }
    SUBCASE("1256, DataType::u32") {
        Value v(std::string("1256"), DataType::u32);
        LIST(v);
    }
    SUBCASE("1256, DataType::u16") {
        Value v(std::string("1256"), DataType::u16);
        LIST(v);
    }

    SUBCASE("14353252321, DataType::i64") {
        Value v(std::string("14353252321"), DataType::i64);
        LIST(v);
    }

    /*
    CHECK(gregDate.getAbsoluteDate() == 737761);
    */
}

#pragma clang diagnostic pop