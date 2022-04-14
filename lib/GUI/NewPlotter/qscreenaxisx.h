#ifndef QSCREENAXISX_H
#define QSCREENAXISX_H

#include <QImage>
#include <QLocale>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QSizeF>

#include "Express_Online_Screen.h"
#include "common/TimeRepresentation.h"

class QScreenAxisX : public QObject {
    Q_OBJECT
   public:
    // AXIS_Y_WIDTH

    explicit QScreenAxisX(const TimeInterval &ti0, const LineProperties &dstx, const Margin &margin,
                          QObject *parent = nullptr);

    ~QScreenAxisX() override;

    void setLabel(const QString &lbl);

    void setMargin(const Margin &margin);

    void setInterval(const TimeInterval &ti, bool zoomed = false);

    void resetInterval();

    void setTime(const RelativeTime &t);

    void setSettings(const LineProperties &dstx, const Margin &margin);

    void rescale();
    void drawAxis();

    QImage *img_;
    QColor clr_ = Qt::white;

    qreal pixel_weight_ = 0;

    QVector<AxisXCutoff> cutoffs_;
    QVector<RelativeTime> scale_;

   public slots:

    void onResize(const qreal &w, const qreal &h);

   private:
    const int type_ = kSourceAxisX;

    TimeInterval ti_;
    LineProperties dstx_;
    Margin margin_;

    RelativeTime t_ = {0};

    QSizeF scene_size_ = QSizeF(kDiagramOffsetLeft + kDiagramOffsetRight + kScreenOffset, kScreenOffset);
    QString label_ = "";

   private slots:

   signals:

    void to_height(const int &h);
};

QString secToHms(const RelativeTime &val, const int &prec = -1);

QString formatValue(const double &val, DataOutputFormat fmt, const int &prec = -1, const bool &trim = false);

int trimZeroes(const double &val, const int &prec = -1);

#endif  // QSCREENAXISX_H
