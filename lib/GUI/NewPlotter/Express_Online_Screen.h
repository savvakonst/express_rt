#ifndef EXPRESS_ONLINE_SCREEN_H
#define EXPRESS_ONLINE_SCREEN_H

#include <QObject>

#include "common/TimeRepresentation.h"

const int32_t kMaxScreenWidth = 2048;

const int32_t kDiagramOffsetLeft = 150;
const int32_t kDiagramOffsetRight = 150;
const int kDiagramMargin = 5;

const qreal kScreenOffset = 50;
const qreal kScreenOffsetTop = 50;
const qreal kScreenOffsetBottom = 50;
const qreal kScreenOffsetLeft = 50;
const qreal kScreenOffsetRight = 50;

const qreal kAxisYHeightMin = 20;
const qreal kAxisYHeightDefault = 40;
const qreal kAxisYHeightMax = 1000;
const qreal kAxisYWidth = 40;
const qreal kAxisYCutoffStep = 20;
const qreal kAxisYDiagInterval = 20;

const int kMaxCutoffNumber = 7;
const int kDefaultDataOutputPrecision = -1;

enum
{
    kSourceUndefined = 65536,
    kSourceMain,
    kSourceScale,
    kSourceMarker,
    kSourceAxisX,
    kSourceMarkerFloat,
    kSourceMarkerAnchor,
    kSourceLabel,
    kSourceHarmonic,
    kSourceBorder
};

enum
{
    kDataOutputFormatFloat = 0,
    kDataOutputFormatInt16,
    kDataOutputFormatInt10,
    kDataOutputFormatInt8,
    kDataOutputFormatInt2,
    kDataOutputFormatAngle,
    kDataOutputFormatTime
};

#pragma pack(push, 1)
struct FileVersion {
    union {
        uint32_t version;
        struct {
            uint8_t v_1;
            uint8_t v_2;
            uint8_t v_3;
            uint8_t v_4;
        };
    };
};
Q_DECLARE_METATYPE(FileVersion);

struct DataOutputFormat {
    int32_t type = kDataOutputFormatFloat;
    int32_t prec = kDefaultDataOutputPrecision;
};
Q_DECLARE_METATYPE(DataOutputFormat);

struct MarkerSimple {
    bool enabled = false;
    qreal x{};
    qreal y{};  // for horizontal line
    RelativeTime t{};
};
Q_DECLARE_METATYPE(MarkerSimple);

struct AxisXCutoff {
    qreal val;
    int pos;
    bool enabled;
    bool reserved[3];
};
Q_DECLARE_METATYPE(AxisXCutoff);

struct TimeInterval {
    RelativeTime bgn{};
    RelativeTime end{};
    // double bgn = 0;    // [s]
    // double end = 100;  // [s]
    int64_t offs = 0;  // reserved
};
Q_DECLARE_METATYPE(TimeInterval);

struct ControlLevel {
    double value = 0;
    bool cross = false;  // 0 -  below, 1 - above
};
Q_DECLARE_METATYPE(ControlLevel);

struct LineProperties {
    int32_t dot_weight = 3;
    int32_t line_weight = 1;
    int32_t interpolation = 2;
    int32_t font_size = 8;
};
Q_DECLARE_METATYPE(LineProperties);

struct Margin {
    int32_t left = int32_t(kScreenOffsetLeft);
    int32_t top = int32_t(kScreenOffsetTop);
    union {
        int32_t right = int32_t(kScreenOffsetRight);
        int32_t width;
    };
    union {
        int32_t bottom = int32_t(kScreenOffsetBottom);
        int32_t height;
    };
};
Q_DECLARE_METATYPE(Margin);

struct Timing {
    RelativeTime width = {0};
    RelativeTime step = {0};
};
Q_DECLARE_METATYPE(Timing);

struct ImageSaving {
    bool auto_save = false;
    int filetype = 0;  // unused
};
Q_DECLARE_METATYPE(ImageSaving);

struct SettingsCommon {
    LineProperties lining;
    Margin margin;
    Timing timing;
    ImageSaving saving;
};
Q_DECLARE_METATYPE(SettingsCommon);

struct SettingsScale {
    LineProperties line;
    DataOutputFormat fmt;
    uint32_t color;
};
Q_DECLARE_METATYPE(SettingsScale);

#pragma pack(pop)

#endif  // EXPRESS_ONLINE_SCREEN_H
