#include "qscreenmarker.h"
#include "qscreenaxisx.h"

//-------------------------------------------------------------------------
QScreenMarker::QScreenMarker(const int &index0, const QPointF &pt,
                             const QSizeF &sz, const LineProperties &dstx0,
                             QWidget *parent)
{
    Q_UNUSED(parent)
    Q_UNUSED(dstx0)

    index_ = index0;

    sceneSize_ = sz;

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
    rt.setHeight(sceneSize_.height() - SCREEN_OFFSET_BOTTOM);
    setRect(rt);

    QPointF ptReal;
    ptReal = pt;
    ptReal.setX(ptReal.x() - MARKER_SHIFT);
    ptReal.setY(0);
    setPos(ptReal);

    checkState(pt.x());

    color_ = 0xE32636;

    // QPixmap *pm;
    img = new QImage;

    drawMarker();
}
//-------------------------------------------------------------------------
QScreenMarker::~QScreenMarker()
{

}
//-------------------------------------------------------------------------
int QScreenMarker::getIndex()
{
    return index_;
}
//-------------------------------------------------------------------------
void QScreenMarker::setTime(const RelativeTime &time)
{
    t_ = time;
}
//-------------------------------------------------------------------------
void QScreenMarker::checkState(const qreal &x)
{
    bool miss = false;
    if ((x < (margin_.left - MARKER_SHIFT)) ||
        (x > (sceneSize_.width() - margin_.right - MARKER_SHIFT))) {
        miss = true;
    }

    enabled_ = !miss;
    valid_ = !miss;
}
//-------------------------------------------------------------------------
void QScreenMarker::setSettings(const LineProperties &dstx)
{
    dstx_ = dstx;
}
//-------------------------------------------------------------------------
void QScreenMarker::setMargin(const Margin &margin)
{
    margin_ = margin;
}
//-------------------------------------------------------------------------
void QScreenMarker::advance(int phase)
{
    Q_UNUSED(phase)
}
//-------------------------------------------------------------------------
int QScreenMarker::type() const
{
    return Type;
}
//-------------------------------------------------------------------------
void QScreenMarker::on_resize(const qreal &w, const qreal &h)
{
    sceneSize_.setWidth(w);
    sceneSize_.setHeight(h);

    QRectF rt = rect();
    rt.setHeight(h - SCREEN_OFFSET_BOTTOM);
    setRect(rt);
    setPos(QPointF(getXbyTime(t_), 0));

    drawMarker();
}
//-------------------------------------------------------------------------
void QScreenMarker::on_setValid(const TimeInterval &ti, const bool &total)
{
    valid_ = true;
    if (t_ < ti.bgn || t_ > ti.end)
        valid_ = false;
    else
        setPos(QPointF(getXbyTime(t_), 0));

    setVisible(valid_);
    setEnabled(valid_);
}
//-------------------------------------------------------------------------
void QScreenMarker::on_indexReduce(const int &src, const int &index)
{
    if (src != Type)
        return;
    if (index_ > index)
        index_--;
}
//-------------------------------------------------------------------------
void QScreenMarker::drawMarker()
{
    QRectF rt = rect();

    QPixmap *pm = new QPixmap(static_cast<int>(MARKER_WIDTH),
                              static_cast<int>(rt.height()));
    // pm->fill(Qt::gray);
    pm->fill(Qt::transparent);
    QPainter *p = new QPainter(pm);

    QFont ft = p->font();
    ft.setPointSize(dstx_.font_size);
    p->setFont(ft);

    QPen pen;
    pen.setStyle(Qt::DashDotDotLine);
    // pen.setStyle(Qt::DashDotLine);
    pen.setColor(color_);
    p->setPen(pen);

    QPointF pt0(MARKER_SHIFT, 0);
    QPointF pt1(MARKER_SHIFT, rt.height());
    p->drawLine(pt0, pt1);

    delete p;

    img->fill(Qt::transparent);
    *img = pm->toImage();

    delete pm;
}
//-------------------------------------------------------------------------
qreal QScreenMarker::getXbyTime(const RelativeTime &time)
{
    qreal x = -1;
    for (int j = 0; j < sceneSize_.width() - SCREEN_OFFSET_RIGHT - 1; j++) {
        if (time >= pScale_->at(j) && time < pScale_->at(j + 1)) {
            x = j;
            break;
        }
    }
    x -= static_cast<int>(MARKER_SHIFT);
    return (x);
}
//-------------------------------------------------------------------------
void QScreenMarker::keyPressEvent(QKeyEvent *event)
{
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

            RelativeTime t0 = pScale_->at(x) - pScale_->at(x - 1);
            RelativeTime t1 = pScale_->at(x + 1) - pScale_->at(x);

            t_ = pScale_->at(x);
        }
        emit to_changed(Type);
    }

    update();
    QGraphicsItem::keyPressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::keyReleaseEvent(QKeyEvent *event)
{
    update();
    QGraphicsItem::keyPressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->button()) {
    case Qt::LeftButton: {
        setFlag(QGraphicsItem::ItemIsSelectable);

        leftPressed_ = true;
        emit to_focused(Type, index_);
    }
    break;
    default:
        ;
    }

    update();
    QGraphicsItem::mousePressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->button()) {
    case Qt::LeftButton: {
        setFlags(flags() & ~QGraphicsItem::ItemIsSelectable);

        QPointF pt = scenePos();
        pt.setY(0);
        setPos(pt);

        checkState(pt.x());
        if (enabled_) {
            int x = static_cast<int>(pt.x() + MARKER_SHIFT);
            t_ = pScale_->at(x);
        }

        emit to_changed(Type);
        leftPressed_ = false;
    }
    break;
    default:
        ;
    }

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->button()) {
    case Qt::LeftButton: {
        setFlags(flags() & ~QGraphicsItem::ItemIsSelectable);

        QPointF pt = scenePos();
        pt.setY(0);
        setPos(pt);

        checkState(pt.x());
        if (enabled_) {
            int x = static_cast<int>(pt.x() + MARKER_SHIFT);
            t_ = pScale_->at(x);
        }

        emit to_changed(Type);
        leftPressed_ = false;
    }
    break;
    default:
        ;
    }

    update();
    QGraphicsItem::mouseMoveEvent(event);
}
//-------------------------------------------------------------------------
void QScreenMarker::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    to_removed(Type, index_);

    update();
    QGraphicsItem::mouseDoubleClickEvent(event);
}
//-------------------------------------------------------------------------
