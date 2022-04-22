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

    void rescale();
    void drawAxis();

    inline RelativeTime getRelativeTime(qreal i) const {
        qreal t = (pixel_weight_ * (i - margin_.left));
        RelativeTime rt{};
        rt.fromDouble(t);
        return rt;
    }

    inline qreal getXPos(const RelativeTime &rt) const { return rt.toDouble() / pixel_weight_ + margin_.left; }

    QImage *img_;
    QColor clr_ = Qt::white;

    qreal pixel_weight_ = 0;

    QVector<AxisXCutoff> cutoffs_;
    QVector<RelativeTime> scale_;

    TimeInterval current_borders_;

   public slots:

    void onResize(const qreal &w, const qreal &h);

   private:
    const int type_ = kSourceAxisX;

    LineProperties lining_;
    Margin margin_;

    RelativeTime t_ = {0};

    QSizeF scene_size_ = QSizeF(kDiagramOffsetLeft + kDiagramOffsetRight + kScreenOffset, kScreenOffset);
    QString label_ = "";

   private slots:

   signals:

    void toHeight(const int &h);
};

QString secToHms(const RelativeTime &val, const int &prec = -1);

QString formatValue(const double &val, DataOutputFormat fmt, const int &prec = -1, const bool &trim = false);

int trimZeroes(const double &val, const int &prec = -1);

#endif  // QSCREENAXISX_H
