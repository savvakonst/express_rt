﻿#include "qscreenmarker.h"

#include "qscreenaxisx.h"

//-------------------------------------------------------------------------
QScreenMarker::QScreenMarker(QScreenAxisX *axis_x, const int &index_0, const QPointF &pt, const QSizeF &sz,
                             const LineProperties &dstx, QWidget *parent)
    : axis_x_(axis_x) {
    Q_UNUSED(parent)

    index_ = index_0;

    sceneSize_ = sz;

    lining_ = dstx;

    // setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFocus();

    setPen(Qt::NoPen);
    setCursor(Qt::PointingHandCursor);
    setBrush(Qt::transparent);

    QRectF rt;
    rt.setTop(0);
    rt.setLeft(0);
    rt.setWidth(MARKER_WIDTH);
    rt.setHeight(sceneSize_.height() - kScreenOffsetBottom);
    setRect(rt);

    QPointF ptReal;
    ptReal = pt;
    ptReal.setX(ptReal.x() - MARKER_SHIFT);
    ptReal.setY(0);
    setPos(ptReal);

    checkState(pt.x());

    color_ = 0xE32636;

    // QPixmap *pm;
    img_ = new QImage;

    relative_time_ = axis_x_->current_borders_.bgn + axis_x_->getRelativeTime(scenePos().x());
    t_ = axis_x_->getRelativeTime(scenePos().x());

    drawMarker();
}
//-------------------------------------------------------------------------
QScreenMarker::~QScreenMarker() {}
//-------------------------------------------------------------------------
int QScreenMarker::getIndex() { return index_; }

//-------------------------------------------------------------------------
void QScreenMarker::checkState(const qreal &x) {
    bool miss = false;
    if ((x < (margin_.left - MARKER_SHIFT)) || (x > (sceneSize_.width() - margin_.right - MARKER_SHIFT))) {
        miss = true;
    }

    enabled_ = !miss;
    valid_ = !miss;
}

//-------------------------------------------------------------------------
void QScreenMarker::setMargin(const Margin &margin) { margin_ = margin; }
//-------------------------------------------------------------------------
void QScreenMarker::advance(int phase) { Q_UNUSED(phase) }
//-------------------------------------------------------------------------
int QScreenMarker::type() const { return Type; }
//-------------------------------------------------------------------------
void QScreenMarker::onResize(const qreal &w, const qreal &h) {
    sceneSize_.setWidth(w);
    sceneSize_.setHeight(h);

    QRectF rt = rect();
    rt.setHeight(h - kScreenOffsetBottom);
    setRect(rt);
    setPos(QPointF(getXbyTime(t_), 0));

    drawMarker();
}
//-------------------------------------------------------------------------
void QScreenMarker::onSetValid(const TimeInterval &ti, const bool &total) {
    valid_ = true;
    if (t_ < ti.bgn || t_ > ti.end) valid_ = false;
    else
        setPos(QPointF(getXbyTime(t_), 0));

    setVisible(valid_);
    setEnabled(valid_);
}
//-------------------------------------------------------------------------
void QScreenMarker::onIndexReduce(const int &src, const int &index) {
    if (src != Type) return;
    if (index_ > index) index_--;
}
//-------------------------------------------------------------------------
void QScreenMarker::drawMarker() {
    QRectF rt = rect();

    auto *pm = new QPixmap(static_cast<int>(MARKER_WIDTH), static_cast<int>(rt.height()));
    // pm->fill(Qt::gray);
    pm->fill(Qt::transparent);
    auto *p = new QPainter(pm);

    QFont ft = p->font();
    ft.setPointSize(lining_.font_size);
    p->setFont(ft);

    QPen pen;
    pen.setStyle(Qt::DashDotDotLine);
    // pen.setStyle(Qt::DashDotLine);
    pen.setColor(color_);
    p->setPen(pen);

    QPointF pt_0(MARKER_SHIFT, 0);
    QPointF pt_1(MARKER_SHIFT, rt.height());
    p->drawLine(pt_0, pt_1);

    delete p;

    img_->fill(Qt::transparent);
    //*img = pm->toImage();

    delete pm;
}
//-------------------------------------------------------------------------
qreal QScreenMarker::getXbyTime(const RelativeTime &time) {
    auto &scale = axis_x_->scale_;

    qreal x = -1;
    for (int j = 0; j < sceneSize_.width() - kScreenOffsetRight - 1; j++) {
        if (time >= scale.at(j) && time < scale.at(j + 1)) {
            x = j;
            break;
        }
    }
    x -= static_cast<int>(MARKER_SHIFT);
    return (x);
}
//-------------------------------------------------------------------------
void QScreenMarker::keyPressEvent(QKeyEvent *event) {
    bool changed = false;

    switch (event->key()) {
    case Qt::Key_Left:
        moveBy(-1, 0);
        changed = true;
        break;
    case Qt::Key_Right:
        moveBy(1, 0);
        changed = true;
        break;
    default:;
    }

    if (changed) {
        checkState(scenePos().x());
        if (enabled_) {
            int x = static_cast<int>(scenePos().x() + MARKER_SHIFT);

            RelativeTime t0 = p_scale_->at(x) - p_scale_->at(x - 1);
            RelativeTime t1 = p_scale_->at(x + 1) - p_scale_->at(x);

            t_ = axis_x_->getRelativeTime(x);
        }
        emit to_changed(Type);
    }

    update();
    QGraphicsItem::keyPressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::keyReleaseEvent(QKeyEvent *event) {
    update();
    QGraphicsItem::keyPressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton: {
        setFlag(QGraphicsItem::ItemIsSelectable);

        left_pressed_ = true;
        emit toFocused(Type, index_);
    } break;
    default:;
    }

    update();
    QGraphicsItem::mousePressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton: {
        setFlags(flags() & ~QGraphicsItem::ItemIsSelectable);

        QPointF pt = scenePos();
        pt.setY(0);
        setPos(pt);

        checkState(pt.x());
        if (enabled_) t_ = axis_x_->getRelativeTime(pt.x() + MARKER_SHIFT);

        emit to_changed(Type);
        left_pressed_ = false;
    } break;
    default:;
    }

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton: {
        setFlags(flags() & ~QGraphicsItem::ItemIsSelectable);

        QPointF pt = scenePos();
        pt.setY(0);
        setPos(pt);

        checkState(pt.x());
        if (enabled_) t_ = axis_x_->getRelativeTime(pt.x() + MARKER_SHIFT);

        emit to_changed(Type);
        left_pressed_ = false;
    } break;
    default:;
    }

    update();
    QGraphicsItem::mouseMoveEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    toRemoved(Type, index_);

    update();
    QGraphicsItem::mouseDoubleClickEvent(event);
}
//-------------------------------------------------------------------------
