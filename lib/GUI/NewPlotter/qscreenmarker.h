#ifndef QSCREENMARKER_H
#define QSCREENMARKER_H

#include <QCursor>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QSizeF>

#include "qscreenaxisx.h"

//#include "Helper_Works2.h"

const qreal MARKER_WIDTH = 5;
const qreal MARKER_SHIFT = 0;

class QScreenMarker : public QObject, public QGraphicsRectItem {
    Q_OBJECT

   public:
    explicit QScreenMarker(const int &index0, const QPointF &pt, const QSizeF &sz, const LineProperties &dstx0,
                           QWidget *parent = nullptr);

    ~QScreenMarker() override;

    int getIndex();

    void setTime(const RelativeTime &time);

    void checkState(const qreal &x);

    void setSettings(const LineProperties &dstx);
    void setMargin(const Margin &margin);

    void advance(int phase) override;

    int type() const override;

    QImage *img;

    int index_ = 0;
    RelativeTime t_ = {0};

    bool valid_ = true;
    bool enabled_ = false;
    bool leftPressed_ = false;

    QVector<RelativeTime> *pScale_;
    QColor color_ = Qt::blue;

   public slots:
    void on_resize(const qreal &w, const qreal &h);

    void on_setValid(const TimeInterval &ti, const bool &total);

    void on_indexReduce(const int &src, const int &index);

   private:
    void drawMarker();

    qreal getXbyTime(const RelativeTime &time);

    const int Type_ = kSourceMarker;

    QSizeF sceneSize_;

    LineProperties dstx_;
    Margin margin_;

   protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

   signals:
    void to_removed(const int &src, const int &index);

    void to_changed(const int &src);

    void to_focused(const int &src, const int &index);

    void to_moved(const int &x);
};

#endif  // QSCREENMARKER_H
