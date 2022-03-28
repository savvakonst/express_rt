#include "qscreenaxisx.h"

//#include "Helper_Works2.h"

//-------------------------------------------------------------------------
QScreenAxisX::QScreenAxisX(const TimeInterval &ti0, const LineProperties &dstx, const Margin &margin, QObject *parent) {
    ti_ = ti0;
    dstx_ = dstx;
    margin_ = margin;

    scene_size_.setWidth(kDiagramOffsetLeft + kDiagramOffsetRight + kScreenOffset);

    QPixmap *pm;
    img_ = new QImage;
    pm = new QPixmap(static_cast<int>(scene_size_.width()), kScreenOffsetBottom);
    pm->fill(clr_);
    *img_ = pm->toImage();
    delete pm;
    pm = nullptr;
}

//-------------------------------------------------------------------------
QScreenAxisX::~QScreenAxisX() {}

//-------------------------------------------------------------------------
void QScreenAxisX::setLabel(const QString &lbl0) { label_ = lbl0; }

//-------------------------------------------------------------------------
void QScreenAxisX::setMargin(const Margin &margin) { margin_ = margin; }

//-------------------------------------------------------------------------
void QScreenAxisX::setInterval(const TimeInterval &ti0, const bool zoomed) {
    ti_ = ti0;

    if (zoomed) rescale();

    drawAxis();
}

//-------------------------------------------------------------------------
void QScreenAxisX::resetInterval() {
    ti_.bgn.ls_integer = 0;
    ti_.end.ls_integer = 60;
}

//-------------------------------------------------------------------------
void QScreenAxisX::setTime(const RelativeTime &t0) {
    t_ = t0;
    if (!b_) t_fisrt_ = t0;

    drawAxis();

    t_last_ = t0;
}

//-------------------------------------------------------------------------
TimeInterval QScreenAxisX::getInterval() { return ti_; }

//-------------------------------------------------------------------------
void QScreenAxisX::setSettings(const LineProperties &dstx, const Margin &margin) {
    dstx_ = dstx;
    margin_ = margin;
}

//-------------------------------------------------------------------------
void QScreenAxisX::onResize(const qreal &w, const qreal &h) {
    scene_size_.setWidth(w);
    scene_size_.setHeight(h);

    rescale();

    drawAxis();
}

//-------------------------------------------------------------------------
void QScreenAxisX::rescale() {
    qreal w = scene_size_.width();
    qreal axisW = scene_size_.width() - margin_.left - margin_.right;

    double dt = ti_.end.toDouble() - ti_.bgn.toDouble();
    int coCount = 3;
    int32_t step = 10;
    if (dt > 10) coCount = dt / 10 + 1;
    else {
        step = 1;
        coCount = dt + 1;
    }
    /*if(coCount < 3){
        coCount *= 5;
        step /= 5;
    }*/

    cutoffs_.clear();
    for (int j = 0; j < coCount; j++) {
        AxisXCutoff co;
        // co.val = ti.end - (10 * j);
        co.val = 0 - (step * j);
        co.enabled = true;

        cutoffs_.push_back(co);
    }

    pixel_weight_ = getLength(ti_).toDouble() / axisW;
    scale_.clear();
    int index = 0;

    int x0 = margin_.left + axisW;

    for (int j = x0; j >= 0; j--) {
        qreal t = 0 - (pixel_weight_ * (x0 - j));
        RelativeTime rt;
        rt.fromDouble(t);

        scale_.push_front(rt);

        if (index < cutoffs_.count()) {
            if (t <= cutoffs_.at(index).val) {
                cutoffs_[index].pos = j;
                index++;
            }
        }
    }

    for (int j = x0 + 1; j < w; j++) {
        // qreal   t_ = ti.end + (pixel_weight_ * (j - x0));
        qreal t = 0 + (pixel_weight_ * (j - x0));
        RelativeTime rt;
        rt.fromDouble(t);
        scale_.push_back(rt);
    }
}

//-------------------------------------------------------------------------
void QScreenAxisX::drawAxis() {
    QPixmap *pm;
    pm = new QPixmap(static_cast<int>(scene_size_.width()), static_cast<int>(scene_size_.height()));
    pm->fill(Qt::transparent);

    QPainter *p = new QPainter(pm);
    p->setPen(Qt::gray);

    QFont ft = p->font();
    ft.setPointSize(dstx_.font_size);
    p->setFont(ft);

    QFontMetrics fm = p->fontMetrics();

    QStringList sl;
    qreal textH = 0;
    if (!label_.isEmpty()) {
        sl = label_.split('\n');
        for (int j = 0; j < sl.count(); ++j) {
            textH += (fm.height() + 4);
        }
    }

    qreal H = scene_size_.height();
    qreal W = scene_size_.width();

    qreal axisH = H - margin_.height - textH;
    qreal axisW = W - margin_.left - margin_.right;
    if (axisW < 100) return;

    p->drawLine(QPointF(margin_.left, axisH), QPointF(axisW + margin_.left, axisH));

    // rescale();

    QLineF ln;
    // bool    flag = true;
    int ct = 0;
    for (AxisXCutoff co : cutoffs_) {
        if (co.enabled) {
            p->setPen(Qt::lightGray);
            ln.setP1(QPointF(co.pos, margin_.top));
            ln.setP2(QPointF(co.pos, axisH));
            p->drawLine(ln);

            p->setPen(Qt::gray);
            ln.setP1(QPointF(co.pos, 6 + axisH));
            ln.setP2(QPointF(co.pos, 1 + axisH));
            p->drawLine(ln);

            p->setPen(Qt::black);

            QString s = "";
            if (!ct) s = QString("%1").arg(secToHMS(ti_.end));  // co.val;
            else
                s = QString("%1").arg(co.val).replace(QLocale(QLocale::English).decimalPoint(),
                                                      QLocale::system().decimalPoint());

            p->drawText(QPointF(co.pos, 20 + axisH), s);

            ct++;
        }
    }

    int hAxisX = margin_.height;
    qreal h = axisH + 3 * fm.height();

    for (int j = 0; j < sl.count(); ++j) {
        QPointF pt(margin_.left, h);
        p->setPen(Qt::black);
        p->drawText(pt, sl.at(j));

        h += (fm.height() + 4);
        hAxisX += (fm.height() + 4);
    }

    emit to_height(hAxisX);

    delete p;

    *img_ = pm->toImage();
    delete pm;
    pm = nullptr;
}

//-------------------------------------------------------------------------
QString QScreenAxisX::secToHMS(const RelativeTime &val, const int &prec) {
    // TODO: Check RelativeTime conversion
    int x = val.ls_integer;
    int hh = static_cast<int>(floor(x / 3600));
    x -= (hh * 3600);
    int mm = static_cast<int>(floor(x / 60));
    x -= (mm * 60);

    double divider = pow(2, 32);
    int64_t fract = 100.0 * val.ms_fractional / divider;

    QString s = QString("%1:%2:%3,%4")
                    .arg(hh, 2, 10, QChar('0'))
                    .arg(mm, 2, 10, QChar('0'))
                    .arg(x, 2, 10, QChar('0'))
                    .arg(fract, 2, 10, QChar('0'));

    return s;
}

//-------------------------------------------------------------------------
RelativeTime QScreenAxisX::getLength(const TimeInterval &ti0) { return (ti_.end - ti_.bgn); }

//-------------------------------------------------------------------------
