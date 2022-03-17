#ifndef EXPRESS_ONLINE_SCREEN_H
#define EXPRESS_ONLINE_SCREEN_H

#include <QObject>

#include "common/TimeRepresentation.h"

const qreal MAX_SCREEM_WIDTH = 2048;

const qreal DIAGRAM_OFFSET_LEFT = 150;
const qreal DIAGRAM_OFFSET_RIGHT = 150;
const int DIAGRAM_MARGIN = 5;

const qreal SCREEN_OFFSET = 50;
const qreal SCREEN_OFFSET_TOP = 50;
const qreal SCREEN_OFFSET_BOTTOM = 50;
const qreal SCREEN_OFFSET_LEFT = 50;
const qreal SCREEN_OFFSET_RIGHT = 50;

const qreal AXIS_Y_HEIGHT_MIN = 20;
const qreal AXIS_Y_HEIGHT_DEFAULT = 40;
const qreal AXIS_Y_HEIGHT_MAX = 1000;
const qreal AXIS_Y_WIDTH = 40;
const qreal AXIS_Y_CUTOFF_STEP = 20;
const qreal AXIS_Y_DIAG_INTERVAL = 20;

const int MAX_CUTOFF_NUMBER = 7;
const int DEFAULT_DATA_OUTPUT_PRECISION = -1;

enum {
    SOURCE_UNDEFINED = 65536,
    SOURCE_MAIN,
    SOURCE_SCALE,
    SOURCE_MARKER,
    SOURCE_AXIS_X,
    SOURCE_MARKER_FLOAT,
    SOURCE_MARKER_ANCHOR,
    SOURCE_LABEL,
    SOURCE_HARMONIC,
    SOURCE_BORDER
};

enum {
    DATA_OUTPUT_FORMAT_FLOAT = 0,
    DATA_OUTPUT_FORMAT_INT_16,
    DATA_OUTPUT_FORMAT_INT_10,
    DATA_OUTPUT_FORMAT_INT_8,
    DATA_OUTPUT_FORMAT_INT_2,
    DATA_OUTPUT_FORMAT_ANGLE,
    DATA_OUTPUT_FORMAT_TIME
};

#pragma pack(push, 1)
typedef struct _FILE_VERSION {
    union {
        uint32_t version;
        struct {
            uint8_t v1;
            uint8_t v2;
            uint8_t v3;
            uint8_t v4;
        };
    };
} FILE_VERSION;
Q_DECLARE_METATYPE(FILE_VERSION);

typedef struct _DataOutputFormat {
    int32_t type = DATA_OUTPUT_FORMAT_FLOAT;
    int32_t prec = DEFAULT_DATA_OUTPUT_PRECISION;
} DataOutputFormat;
Q_DECLARE_METATYPE(DataOutputFormat);

typedef struct _MarkerSimple {
    bool enabled = false;
    qreal x;
    qreal y;      // for horizontal line
    RelativeTime t;
} MarkerSimple;
Q_DECLARE_METATYPE(MarkerSimple);

typedef struct _AxisXCutoff {
    qreal val;
    int pos;
    bool enabled;
    bool reserved[3];
} AxisXCutoff;
Q_DECLARE_METATYPE(AxisXCutoff);

typedef struct _TimeInterval {
    RelativeTime bgn;
    RelativeTime end;
    //double bgn = 0;    // [s]
    //double end = 100;  // [s]
    int64_t offs = 0;    // reserved
} TimeInterval;
Q_DECLARE_METATYPE(TimeInterval);

typedef struct _ControlLevel {
    double value = 0;
    bool cross = false;         // 0 -  below, 1 - above
} ControlLevel;
Q_DECLARE_METATYPE(ControlLevel);

typedef struct _LineProperties {
    int32_t dot_weight = 3;
    int32_t line_weight = 1;
    int32_t interpolation = 2;
    int32_t font_size = 8;
} LineProperties;
Q_DECLARE_METATYPE(LineProperties);

typedef struct _Margins {
    int32_t left = SCREEN_OFFSET_LEFT;
    int32_t top = SCREEN_OFFSET_TOP;
    union {
        int32_t right = SCREEN_OFFSET_RIGHT;
        int32_t width;
    };
    union {
        int32_t bottom = SCREEN_OFFSET_BOTTOM;
        int32_t height;
    };
} Margin;
Q_DECLARE_METATYPE(Margin);

typedef struct _Timing {
    RelativeTime width = {0};
    RelativeTime step = {0};
} Timing;
Q_DECLARE_METATYPE(Timing);

typedef struct _ImageSaving {
    bool autosave = false;
    int filetype = 0;   // unused
} ImageSaving;
Q_DECLARE_METATYPE(ImageSaving);

typedef struct _SettingsCommon {
    LineProperties lining;
    Margin  margin;
    Timing timing;
    ImageSaving saving;
} SettingsCommon;
Q_DECLARE_METATYPE(SettingsCommon);

typedef struct _SettingsScale {
    LineProperties      line;
    DataOutputFormat    fmt;
    uint32_t            color;

} SettingsScale;

Q_DECLARE_METATYPE(SettingsScale);

#pragma pack(pop)

#endif // EXPRESS_ONLINE_SCREEN_H
