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

    ~QScreenAxisX();

    void setLabel(const QString &lbl0);

    void setMargin(const Margin &margin);

    void setInterval(const TimeInterval &ti0, const bool zoomed = false);

    void resetInterval();

    void setTime(const RelativeTime &t0);

    TimeInterval getInterval();

    void setSettings(const LineProperties &dstx, const Margin &margin);

    void rescale();
    void drawAxis();

    QString secToHMS(const RelativeTime &val, const int &prec = -1);

    QImage *img_;
    QColor clr_ = Qt::white;

    qreal pixel_weight_ = 0;

    QVector<AxisXCutoff> cutoffs_;
    QVector<RelativeTime> scale_;

   public slots:

    void onResize(const qreal &w, const qreal &h);

   private:
    RelativeTime getLength(const TimeInterval &ti);

    const int type_ = kSourceAxisX;

    TimeInterval ti_;
    // int32_t step_ = 10;
    LineProperties dstx_;
    Margin margin_;

    RelativeTime t_ = {0};  // s * 1024
    RelativeTime t_last_ = {0};
    RelativeTime t_fisrt_ = {0};
    bool b_ = false;

    QSizeF scene_size_ = QSizeF(kDiagramOffsetLeft + kDiagramOffsetRight + kScreenOffset, kScreenOffset);
    QString label_ = "";

   private slots:

   signals:

    void to_height(const int &h);
};

#endif  // QSCREENAXISX_H
