#include "qscreenaxisx.h"

//#include "Helper_Works2.h"

//-------------------------------------------------------------------------
QScreenAxisX::QScreenAxisX(const TimeInterval &ti, const LineProperties &dstx, const Margin &margin, QObject *parent) {
    ti_ = ti;
    dstx_ = dstx;
    margin_ = margin;

    scene_size_.setWidth(kDiagramOffsetLeft + kDiagramOffsetRight + kScreenOffset);

    QPixmap *pm;
    img_ = new QImage;
    pm = new QPixmap(static_cast<int>(scene_size_.width()), kScreenOffsetBottom);
    pm->fill(clr_);
    *img_ = pm->toImage();
    delete pm;
}

//-------------------------------------------------------------------------
QScreenAxisX::~QScreenAxisX() {}

//-------------------------------------------------------------------------
void QScreenAxisX::setLabel(const QString &lbl) { label_ = lbl; }

//-------------------------------------------------------------------------
void QScreenAxisX::setMargin(const Margin &margin) { margin_ = margin; }

//-------------------------------------------------------------------------
void QScreenAxisX::setInterval(const TimeInterval &ti, bool zoomed) {
    ti_ = ti;

    if (zoomed) rescale();

    drawAxis();
}

//-------------------------------------------------------------------------
void QScreenAxisX::resetInterval() {
    ti_.bgn.ls_integer = 0;
    ti_.end.ls_integer = 60;
}

//-------------------------------------------------------------------------
void QScreenAxisX::setTime(const RelativeTime &t) {
    t_ = t;

    drawAxis();
}

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
    qreal axis_w = scene_size_.width() - margin_.left - margin_.right;

    auto dt = (ti_.end - ti_.bgn);

    int step = 10;
    int co_count = (dt.ls_integer >= 10) ? dt.ls_integer / 10 + 1 : (step = 1, dt.ls_integer + 1);

    cutoffs_.clear();
    for (int j = 0; j < co_count; j++) {
        AxisXCutoff co{};

        co.val = 0 - (step * j);
        co.enabled = true;

        cutoffs_.push_back(co);
    }

    pixel_weight_ = dt.toDouble() / axis_w;
    scale_.clear();
    int index = 0;

    int x = int(margin_.left + axis_w);

    for (int j = x; j >= 0; j--) {
        qreal t = 0 - (pixel_weight_ * (x - j));
        RelativeTime rt{};
        rt.fromDouble(t);

        scale_.push_front(rt);

        if (index < cutoffs_.count()) {
            if (t <= cutoffs_.at(index).val) {
                cutoffs_[index].pos = j;
                index++;
            }
        }
    }

    for (int j = x + 1; j < w; j++) {
        qreal t = 0 + (pixel_weight_ * (j - x));
        RelativeTime rt{};
        rt.fromDouble(t);
        scale_.push_back(rt);
    }
}

//-------------------------------------------------------------------------
void QScreenAxisX::drawAxis() {
    QPixmap *pm;
    pm = new QPixmap(static_cast<int>(scene_size_.width()), static_cast<int>(scene_size_.height()));
    pm->fill(Qt::transparent);

    auto *p = new QPainter(pm);
    p->setPen(Qt::gray);

    QFont ft = p->font();
    ft.setPointSize(dstx_.font_size);
    p->setFont(ft);

    QFontMetrics fm = p->fontMetrics();

    QStringList sl;
    qreal text_h = 0;
    if (!label_.isEmpty()) {
        sl = label_.split('\n');
        for (int j = 0; j < sl.count(); ++j) {
            text_h += (fm.height() + 4);
        }
    }

    qreal scene_h = scene_size_.height();
    qreal scene_w = scene_size_.width();

    qreal axis_h = scene_h - margin_.height - text_h;
    qreal axis_w = scene_w - margin_.left - margin_.right;
    if (axis_w < 100) return;

    p->drawLine(QPointF(margin_.left, axis_h), QPointF(axis_w + margin_.left, axis_h));

    // rescale();

    QLineF ln;
    // bool    flag = true;
    int ct = 0;
    for (AxisXCutoff co : cutoffs_) {
        if (co.enabled) {
            p->setPen(Qt::lightGray);
            ln.setP1(QPointF(co.pos, margin_.top));
            ln.setP2(QPointF(co.pos, axis_h));
            p->drawLine(ln);

            p->setPen(Qt::gray);
            ln.setP1(QPointF(co.pos, 6 + axis_h));
            ln.setP2(QPointF(co.pos, 1 + axis_h));
            p->drawLine(ln);

            p->setPen(Qt::black);

            QString s;
            if (!ct) s = QString("%1").arg(secToHms(ti_.end));  // co.val;
            else
                s = QString("%1").arg(co.val).replace(QLocale(QLocale::English).decimalPoint(),
                                                      QLocale::system().decimalPoint());

            p->drawText(QPointF(co.pos, 20 + axis_h), s);

            ct++;
        }
    }

    int h_axis_x = margin_.height;
    qreal h = axis_h + 3 * fm.height();

    for (int j = 0; j < sl.count(); ++j) {
        QPointF pt(margin_.left, h);
        p->setPen(Qt::black);
        p->drawText(pt, sl.at(j));

        h += (fm.height() + 4);
        h_axis_x += (fm.height() + 4);
    }

    emit to_height(h_axis_x);

    delete p;

    *img_ = pm->toImage();
    delete pm;
}

//-------------------------------------------------------------------------
QString secToHms(const RelativeTime &val, const int &prec) {
    // TODO: Check RelativeTime conversion
    int x = val.ls_integer;
    int hh = static_cast<int>(floor(x / 3600));
    x -= (hh * 3600);
    int mm = static_cast<int>(floor(x / 60));
    x -= (mm * 60);

    int64_t fractional = 100 * val.ms_fractional / int64_t(pow(2, 32));

    QString s = QString("%1:%2:%3,%4")
                    .arg(hh, 2, 10, QChar('0'))
                    .arg(mm, 2, 10, QChar('0'))
                    .arg(x, 2, 10, QChar('0'))
                    .arg(fractional, 2, 10, QChar('0'));

    return s;
}

QString formatValue(const double &val, const DataOutputFormat fmt, const int &prec, const bool &trim) {
    QString s;

    switch (fmt.type) {
    case kDataOutputFormatInt16: {
        int i_val = static_cast<int>(val);
        s = QString("%1").arg(i_val, 0, 16);
    } break;
    case kDataOutputFormatInt10: {
        int i_val = static_cast<int>(val);
        s = QString("%1").arg(i_val);
    } break;
    case kDataOutputFormatInt8: {
        int i_val = static_cast<int>(val);
        s = QString("%1").arg(i_val, 0, 8);
    } break;
    case kDataOutputFormatInt2: {
        int i_val = static_cast<int>(val);
        s = QString("%1").arg(i_val, 0, 2);
    } break;
    case kDataOutputFormatAngle: {
        int gg = static_cast<int>(floor(val));

        double ms = 60 * (val - gg);

        int mm = static_cast<int>(floor(ms));

        double ss = 60 * (ms - mm);

        if (fmt.prec == kDefaultDataOutputPrecision) {
            if (prec >= 0) {
                int precx = trimZeroes(ss, prec);
                s = QString("%1°%2'%3\"").arg(gg, 2, 10, QChar('0')).arg(mm, 2, 10, QChar('0')).arg(ss, 0, 'f', precx);
            } else
                s = QString("%1°%2'%3\"").arg(gg, 2, 10, QChar('0')).arg(mm, 2, 10, QChar('0')).arg(ss);
        } else
            s = QString("%1°%2'%3\"").arg(gg, 2, 10, QChar('0')).arg(mm, 2, 10, QChar('0')).arg(ss, 0, 'f', fmt.prec);

        s.replace(QLocale(QLocale::English).decimalPoint(), QLocale::system().decimalPoint());
    } break;
    case kDataOutputFormatTime: {
        double x = val;
        int hh = static_cast<int>(floor(x / 3600));
        x -= (hh * 3600);
        int mm = static_cast<int>(floor(x / 60));
        x -= (mm * 60);

        if (fmt.prec == kDefaultDataOutputPrecision) {
            if (prec >= 0) {
                int precx = trimZeroes(x, prec);
                s = QString("%1:%2:%3").arg(hh, 2, 10, QChar('0')).arg(mm, 2, 10, QChar('0')).arg(x, 0, 'f', precx);
            } else
                s = QString("%1:%2:%3").arg(hh, 2, 10, QChar('0')).arg(mm, 2, 10, QChar('0')).arg(x);
        } else
            s = QString("%1:%2:%3").arg(hh, 2, 10, QChar('0')).arg(mm, 2, 10, QChar('0')).arg(x, 0, 'f', fmt.prec);

        s.replace(QLocale(QLocale::English).decimalPoint(), QLocale::system().decimalPoint());
    } break;
    default:
        if (fmt.prec == kDefaultDataOutputPrecision) {
            if (prec >= 0) {
                int precx = trimZeroes(val, prec);
                s = QString("%1").arg(val, 0, 'f', precx);
            } else
                s = QString("%1").arg(val, 0, 'f', 0);
        } else
            s = QString("%1").arg(val, 0, 'f', fmt.prec);

        s.replace(QLocale(QLocale::English).decimalPoint(), QLocale::system().decimalPoint());
    }
    return s;
}

int trimZeroes(const double &val_0, const int &prec) {
    double val = val_0;
    int ct = 0;

    while (val - floor(val) > 0) {
        val *= 10;
        ct++;

        if (ct > prec) break;
    }

    QString s = QString("%1").arg(val_0, 0, 'f', ct);

    QStringList list = s.split(".");

    if (list.count() < 2) return 0;

    QString ss = list.last();

    while (!ss.isEmpty()) {
        QChar cz = ss.at(ss.count() - 1);

        if (cz.toLatin1() != '0') break;

        ss.chop(1);
        ct--;
    }

    return ct;
}