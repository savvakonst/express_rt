#include "qscreenscale.h"

#include "convtemplate/Parameter_ifs.h"
//-------------------------------------------------------------------------
QScreenScale::QScreenScale(Reader_ifs *reader, const int &index, const QSizeF &sz, const LineProperties &dstx,
                           const Margin &margin, QWidget *parent) {
    Q_UNUSED(parent)

    index_ = index;
    scene_size_ = sz;

    auto parameter = reader->getParameter();
    if (parameter) {
        s_label_ = parameter->getPropertyAsTxt("common/name").data();

        QString s_unit = parameter->getPropertyAsTxt("common/units").data();
        if (!s_unit.isEmpty()) s_label_ += QString(", ") + s_unit;
    }

    dstx_ = dstx;
    margin_ = margin;
    color_ = diag_colors_.at(rand() % (diag_colors_.count() - 1));

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFocus();

    setPen(QColor(color_));
    setPen(Qt::NoPen);
    setCursor(Qt::PointingHandCursor);
    setBrush(Qt::transparent);

    QRectF r;
    r.setTop(0);
    r.setLeft(0);
    r.setWidth(kAxisYWidth);
    r.setHeight(kAxisYHeightDefault);

    // Position

    int h = int(scene_size_.height()) - margin_.top;
    int dx = kAxisYWidth + kAxisYDiagInterval;
    int dy = kAxisYHeightDefault + kAxisYDiagInterval;
    qreal n = floor(h / dy);

    int c = int(float(index_) / n);
    int i = int(float(index_) - n * c);

    qreal x = (dx * c) + kScreenOffsetLeft;
    qreal y = (dy * i) + margin_.top;

    setRect(r);
    setPos(x, y);

    /*QSizeF   sceneSize;
    sceneSize.setWidth(sceneW);
    sceneSize.setHeight(sceneH);*/

    img_diag_ = new QImage;
    img_scale_ = new QImage;

    // Popup Menu
    act_settings_ = new QAction(QIcon("://settings"), tr("Настройки"), this);
    connect(act_settings_, &QAction::triggered, this, &QScreenScale::onSettingsShow);

    act_level_add_ = new QAction(QIcon("://"), tr("Добавить уровень"), this);
    connect(act_level_add_, &QAction::triggered, this, &QScreenScale::onAddLevelBegin);

    act_align_t_ = new QAction(tr("Поверху"), this);
    connect(act_align_t_, &QAction::triggered, this, &QScreenScale::onAlignT);

    act_align_b_ = new QAction(tr("Понизу"), this);
    connect(act_align_b_, &QAction::triggered, this, &QScreenScale::onAlignB);

    act_align_l_ = new QAction(tr("Слева"), this);
    connect(act_align_l_, &QAction::triggered, this, &QScreenScale::onAlignL);

    act_align_r_ = new QAction(tr("Справа"), this);
    connect(act_align_r_, &QAction::triggered, this, &QScreenScale::onAlignR);

    act_align_h_ = new QAction(tr("По высоте"), this);
    connect(act_align_h_, &QAction::triggered, this, &QScreenScale::onAlignH);

    menu_align_ = new QMenu(tr("Выровнять"));
    menu_align_->addAction(act_align_t_);
    menu_align_->addAction(act_align_b_);
    menu_align_->addAction(act_align_l_);
    menu_align_->addAction(act_align_r_);
    menu_align_->addAction(act_align_h_);

    act_scale_auto_ = new QAction(tr("Авто"), this);
    // actScaleAuto->setShortcut(Qt::Key_Y);
    connect(act_scale_auto_, &QAction::triggered, this, &QScreenScale::onScaleAuto);

    act_scale_fix_ = new QAction(tr("Зафиксировать"), this);
    // actScaleFix->setShortcut(Qt::CTRL + Qt::Key_Y);
    connect(act_scale_fix_, &QAction::triggered, this, &QScreenScale::onScaleFix);

    menu_scale_ = new QMenu(tr("Границы"));
    menu_scale_->addAction(act_scale_auto_);
    menu_scale_->addAction(act_scale_fix_);

    act_remove_ = new QAction(QIcon("://close"), tr("Удалить"), this);
    connect(act_remove_, &QAction::triggered, this, &QScreenScale::onRemove);

    setTemporaryValues(0, 0);  //

    // Example
    // p_ = createNewPrmBufferSimulator(16);

    ControlLevel lvl;
    // lvl.cross = true;
    // lvl.value = 0.75;
    // levels_.push_back(lvl);
    lvl.cross = false;
    lvl.value = -0.75;
    levels_.push_back(lvl);

    SinglePrm prm;

    prm.reader = reader;
    prm.buffer = new Reader_ifs::Point[kMaxScreenWidth];
    prm.plevels = &levels_;

    parameters_.push_back(std::move(prm));
    //

    drawScale();
}

//-------------------------------------------------------------------------
QScreenScale::~QScreenScale() = default;
//-------------------------------------------------------------------------
int QScreenScale::getIndex() const { return index_; }
//-------------------------------------------------------------------------
void QScreenScale::setTemporaryValues(const double val_min, double const val_max) {
    stat_.ct = 1;
    stat_.val_max = val_max;
    stat_.val_min = val_min;

    maximum_.value = val_max;
    minimum_.value = val_min;
}
//-------------------------------------------------------------------------
void QScreenScale::setScale(QVector<RelativeTime> *xs) { p_scale_ = xs; }
//-------------------------------------------------------------------------
void QScreenScale::setTiming(const TimeInterval &ti, const RelativeTime &step) {
    ti_ = ti;
    interval_on_ = true;

    drawDiag();
    drawScale();
}

//-------------------------------------------------------------------------
void QScreenScale::setMargins(const Margin &margin) { margin_ = margin; }
//-------------------------------------------------------------------------
void QScreenScale::advance(int phase) { Q_UNUSED(phase) }
//-------------------------------------------------------------------------
int QScreenScale::type() const { return type_; }
//-------------------------------------------------------------------------
void QScreenScale::recountScaleValues(const int &w, AxisYStatistics &stat, Reader_ifs::Chunk *c_ptr) {
    memset(&stat, 0, sizeof(stat));

    AxisXyDot last_dot;
    QVector<AxisXyDot> dots;
    dots.reserve((int)kMaxScreenWidth);

    while (c_ptr) {
        Reader_ifs::Point *pb = c_ptr->first_point_;
        for (size_t i = 0; i < c_ptr->number_of_points_; i++) {
            if (!stat.ct) {
                stat.val_min = pb[i].min;
                stat.val_max = pb[i].max;
            } else {
                stat.val_min = std::min(stat.val_min, pb[i].min);
                stat.val_max = std::max(stat.val_max, pb[i].max);
            }
            stat.ct++;
            // double  sum = stat.m;
            // stat.m  = stat.m + (pb[i]. - stat.m) / stat.ct;          // Welford
            // stat.d  = stat.d + (value - stat.m) * (value - sum);

            while (dots.count() < pb[i].pos) {
                AxisXyDot dot = last_dot;
                dot.x = dots.count();
                // dot.ghost = true;
                dots.push_back(AxisXyDot());
            }

            AxisXyDot dot;
            dot.val_max = pb[i].max;
            dot.val_min = pb[i].min;
            dot.x = int(pb[i].pos);
            dot.ct++;
            dot.ghost = false;

            for (auto lvl : levels_) {
                if (lvl.cross) {
                    if (dot.val_max >= lvl.value) dot.warning = true;
                } else {
                    if (dot.val_min <= lvl.value) dot.warning = true;
                }
            }

            dots.push_back(dot);
            last_dot = dot;
        }

        c_ptr = c_ptr->next_;
    }
    stat_.val_min = std::min(stat_.val_min, stat.val_min);
    stat_.val_max = std::max(stat_.val_max, stat.val_max);

    while (dots.count() < w) {
        AxisXyDot dot = last_dot;
        dot.x = dots.count();
        // dot.ghost = true;
        dots.push_back(AxisXyDot());
    }

    list_dots_.push_back(dots);
}

//-------------------------------------------------------------------------
void QScreenScale::drawDiag() {
    if (!interval_on_) return;

    int scale_h = static_cast<int>(rect().height());

    int image_w = static_cast<int>(scene_size_.width()) - margin_.left - margin_.right;
    if (image_w < 100) return;

    list_dots_.clear();
    warning_ = false;

    RelativeTime dt = ti_.end - ti_.bgn;
    dt.ms_fractional = 0;

    if (!is_paused_) {
        memset(&stat_, 0, sizeof(stat_));
        for (auto &p : parameters_) {
            Borders b = p.reader->getAvailableBorders();

            b.begin = b.end - dt;

            p.chunks = p.reader->getPoints(b, p.buffer, image_w);
            recountScaleValues(image_w, p.stat, p.chunks.get());
        }
    }

    double scale_min = stat_.val_min;
    if (!minimum_.automatic) scale_min = minimum_.value;

    double scale_max = stat_.val_max;
    if (!maximum_.automatic) scale_max = maximum_.value;

    if (scale_min >= scale_max) {
        scale_min -= 1;
        scale_max += 1;
    }

    minimum_.value = scale_min;
    maximum_.value = scale_max;

    double data_min = stat_.val_min;
    double data_max = stat_.val_max;
    if (data_min >= data_max) {
        data_min -= 0.5;
        data_max += 0.5;
    }

    double ratio = (data_max - data_min) / (scale_max - scale_min);

    int image_h = static_cast<int>(floor((ratio * scale_h) + 0.5));

    double k = image_h / (data_max - data_min);

    double shift_k = scale_h / (scale_max - scale_min);

    img_shift_y_ = static_cast<int>(floor((scale_max - data_max) * shift_k + 0.5));

    auto *pm = new QPixmap(image_w + kDiagramMargin, image_h + kDiagramMargin + kDiagramMargin);
    pm->fill(Qt::transparent);
    auto *painter = new QPainter(pm);

    QFont ft = painter->font();
    ft.setPointSize(dstx_.font_size);
    painter->setFont(ft);

    QPen pen;

    // parameter
    pen.setColor(QColor(color_));
    pen.setWidth(dstx_.line_weight);
    painter->setPen(pen);
    /*
    for (auto &prm : parameters_) {
        auto const *c = prm.chunks.get();
        while (c) {
            for (auto p = c->first_point_, p_end = c->first_point_ + c->number_of_points_; p < p_end; p++) {
                auto y_min = image_h - int(floor((p->min - data_min) * k + 0.5)) + kDiagramMargin;
                auto y_max = image_h - int(floor((p->max - data_min) * k + 0.5)) + kDiagramMargin;

                painter->drawLine(p->pos, y_min, p->pos, y_max);
            }
            c = c->next_;
        }
    }

    */

    for (auto &dots : list_dots_) {
        AxisXyDot last_dot;

        last_dot.x = -1;
        for (auto &d : dots) {
            if (d.ghost) continue;

            d.y0 = image_h - static_cast<int>(floor((d.val_min - data_min) * k + 0.5)) + kDiagramMargin;
            d.y1 = image_h - static_cast<int>(floor((d.val_max - data_min) * k + 0.5)) + kDiagramMargin;

            // if(last_dot.x >= 0 && !last_dot.ghost)
            //     painter->drawLine(last_dot.x, last_dot.y1, d.x, d.y1);
            painter->drawLine(d.x, d.y0, d.x, d.y1);

            last_dot = d;
        }
    }

    // levels points
    pen.setColor(QColor(Qt::red));
    pen.setWidth(5);
    painter->setPen(pen);

    for (const auto lvl : levels_)
        for (auto &dots : list_dots_) {
            AxisXyDot last_dot;

            last_dot.x = -1;
            for (auto &d : dots) {
                if (d.ghost) continue;

                if (lvl.cross) {
                    if (d.val_max > lvl.value) {
                        int y = image_h - static_cast<int>(floor((d.val_max - data_min) * k + 0.5)) + kDiagramMargin;
                        painter->drawPoint(d.x, y);
                    }
                } else {
                    if (d.val_min < lvl.value) {
                        int y = image_h - static_cast<int>(floor((d.val_min - data_min) * k + 0.5)) + kDiagramMargin;
                        painter->drawPoint(d.x, y);
                    }
                }
            }
        }

    // Levels
    pen.setWidth(1);
    pen.setColor(QColor(color_).darker(150));
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    for (const auto lvl : levels_) {
        int y = image_h - static_cast<int>(floor((lvl.value - data_min) * k + 0.5));
        painter->drawLine(0, y + kDiagramMargin, image_w, y + kDiagramMargin);
    }

    delete painter;

    img_diag_->fill(Qt::transparent);
    *img_diag_ = pm->toImage();

    delete pm;
}

//-------------------------------------------------------------------------
void QScreenScale::drawScale() {
    QRectF rt = rect();

    auto *pm = new QPixmap(static_cast<int>(rt.width()), static_cast<int>(rt.height() + 1));
    pm->fill(Qt::transparent);

    auto *painter = new QPainter(pm);

    QFont ft = painter->font();
    ft.setPointSize(dstx_.font_size);
    painter->setFont(ft);

    QFontMetrics fm = painter->fontMetrics();
    int txt_h = fm.height();

    int h = static_cast<int>(rt.height());

    double val_max = maximum_.automatic ? stat_.val_max : maximum_.value;
    double val_min = minimum_.automatic ? stat_.val_min : minimum_.value;

    if (val_max <= val_min) {
        val_max += 1.0001;
        val_min -= 1.0001;
    }

    painter->fillRect(0, 0, 10, (h - 1), QBrush(Qt::white));

    painter->setPen(QColor(Qt::lightGray));
    painter->drawLine(1, 0, 6, 0);
    painter->drawLine(1, h, 6, h);
    painter->setPen(QColor(color_));
    painter->drawLine(6, 0, 6, h);

    // Cutoffs Definition
    qreal rng = val_max - val_min;
    if (rng <= 0) rng = 1;
    qreal rng_abs = abs(rng);
    qreal rng_pow = floor(log10(rng_abs) + 0.5);

    qreal step = pow(10, rng_pow);
    qreal nmbr = step_.value;

    qreal dot_weight = rng_abs / h;

    qreal y_from;
    int last_pos = 0;

    if (step_.automatic) {
        nmbr = floor(rng_abs / step);
        while ((nmbr > 10) || (nmbr < 2)) {
            if (nmbr > 10) step *= 10;
            else
                step /= 2;
            nmbr = floor(rng_abs / step);
        }

        y_from = ceil(val_max / step) * step;
    } else {
        qreal p_max = std::max(log10(val_max), log10(val_min));
        p_max = floor(p_max);

        y_from = ceil(floor(val_max) / pow(10, p_max)) * pow(10, p_max);

        if (nmbr == 0) nmbr = 1;

        step = (rng_abs / (nmbr));

        qreal p = floor(log10(step));

        step = floor(step / pow(10, p)) * pow(10, p);

        auto nx = int(rng_abs / step);
        while (nx / nmbr > 2) {
            step *= 2;
            nx = int(rng_abs / step);
        }
    }

    qreal y = y_from;
    int ct = 0;

    cut_off_.clear();
    while (y >= val_min) {
        if (y <= val_max) {
            AxisYCutoff co{};
            memset(&co, 0, sizeof(co));
            co.val = y;
            co.pos = static_cast<int>(h - floor(((y - val_min) / dot_weight) + 0.5));
            if (cut_off_.empty()) {
                co.enabled = true;
                last_pos = co.pos;
            } else {
                if ((co.pos - last_pos) >= txt_h) {
                    co.enabled = true;
                    last_pos = co.pos;
                }
            }
            cut_off_.push_back(co);
        }

        ct++;
        y = y_from - (ct * step);
    }

    if (step_.automatic) step_.value = qreal(cut_off_.size());

    precision_ = -1;
    rng_pow = static_cast<int>(floor(log10(rng_abs)));
    if (rng_pow > 0) precision_ = 0;
    else if (rng_pow < -8)
        precision_ = 9;
    else
        precision_ = static_cast<int>(fabs(rng_pow) + 1);

    precision_scale_ = precision_;
    int step_pow = static_cast<int>(floor(log10(step)));
    if (step_pow < rng_pow) precision_scale_++;

    for (auto co : cut_off_) {
        painter->drawLine(1, co.pos, 6, co.pos);
    }

    delete painter;

    img_scale_->fill(Qt::transparent);
    *img_scale_ = pm->toImage();

    delete pm;
}

//-------------------------------------------------------------------------
void QScreenScale::createPopupMenu(const QPoint &pt) {
    bool scale_auto_state = maximum_.automatic & minimum_.automatic;

    act_scale_auto_->setVisible(!scale_auto_state);
    act_scale_fix_->setVisible(scale_auto_state);

    auto *menu = new QMenu();
    menu->addAction(act_settings_);
    menu->addSeparator();
    menu->addMenu(menu_align_);
    menu->addSeparator();
    menu->addMenu(menu_scale_);
    menu->addSeparator();
    menu->addAction(act_level_add_);
    menu->addSeparator();
    menu->addAction(act_remove_);

    menu->exec(pt, act_remove_);
}

//-------------------------------------------------------------------------
void QScreenScale::onRemove() {
    emit toRemoved(type_, index_);

    deleteLater();
}

//-------------------------------------------------------------------------
void QScreenScale::onResize(const qreal &w, const qreal &h) {
    scene_size_.setWidth(w);
    scene_size_.setHeight(h);

    drawDiag();
}
//-------------------------------------------------------------------------
void QScreenScale::onSetX(const int &index, const int &x) {
    if (index == index_) X_ = x;
}
//-------------------------------------------------------------------------
void QScreenScale::onIndexReduce(const int &src, const int &index) {
    if (src != type_) return;
    if (index_ > index) index_--;
}
//-------------------------------------------------------------------------
void QScreenScale::onSettingsShow() { emit toTuned(type_, index_); }
//-------------------------------------------------------------------------
void QScreenScale::onAddLevelBegin() const {
    auto *fl = new QFormLevel(ControlLevel(), false);

    connect(fl, &QFormLevel::to_confirmed, this, &QScreenScale::onAddLevelEnd);

    fl->show();
}
//-------------------------------------------------------------------------
void QScreenScale::onAddLevelEnd(const ControlLevel &lvl, const bool &flag, const int &index) {
    if (flag) levels_[index] = lvl;
    else  // new
        levels_.push_back(lvl);
}
//-------------------------------------------------------------------------
void QScreenScale::onSetPause(const bool is_paused) { is_paused_ = is_paused; }
//-------------------------------------------------------------------------
void QScreenScale::changeScaleBorder(const bool &high, const int &delta) {
    if (delta == 0) return;

    maximum_.automatic = false;
    minimum_.automatic = false;

    qreal val;
    qreal ref;
    if (high) {
        val = maximum_.value;
        ref = minimum_.value;
    } else {
        val = minimum_.value;
        ref = maximum_.value;
    }
    int sign = (val < 0) ? -1 : 1;

    qreal rng = maximum_.value - minimum_.value;
    int prng = 0;
    if (rng > 0) prng = static_cast<int>(floor(log10(rng)));

    qreal step = pow(10, prng);
    qreal st_10 = step / 10.0;
    qreal st_100 = step / 100.0;
    qreal val_0 = floor(val / step) * step;

    while (val_0 < fabs(val)) {
        val_0 += st_10;
        if ((val_0 + st_100) >= fabs(val)) break;
    }
    val_0 *= sign;

    if (high) {
        if (delta > 0) maximum_.value = val_0 + st_10;
        else {
            qreal val_1 = val_0 - st_10;
            if (val_1 > ref) maximum_.value = val_1;
        }
    } else {
        if (delta < 0) minimum_.value = val_0 - st_10;
        else {
            qreal val_1 = val_0 + st_10;
            if (val_1 < ref) minimum_.value = val_1;
        }
    }
}

//-------------------------------------------------------------------------
void QScreenScale::onAlignT() { emit toAlign(type_, index_, 0); }
//-------------------------------------------------------------------------
void QScreenScale::onAlignB() { emit toAlign(type_, index_, 1); }
//-------------------------------------------------------------------------
void QScreenScale::onAlignL() { emit toAlign(type_, index_, 2); }
//-------------------------------------------------------------------------
void QScreenScale::onAlignR() { emit toAlign(type_, index_, 3); }
//-------------------------------------------------------------------------
void QScreenScale::onAlignH() { emit toAlign(type_, index_, 4); }
//-------------------------------------------------------------------------
void QScreenScale::onScaleAuto() {
    minimum_.automatic = true;
    maximum_.automatic = true;

    drawDiag();
    drawScale();
}
//-------------------------------------------------------------------------
void QScreenScale::onScaleFix() {
    minimum_.automatic = false;
    maximum_.automatic = false;
}
//-------------------------------------------------------------------------
void QScreenScale::keyPressEvent(QKeyEvent *event) {
    int speed = 5;
    if (event->modifiers() & Qt::ShiftModifier) speed = 1;

    QPointF pt = scenePos();

    switch (event->key()) {
    case Qt::Key_Left:
        if (pt.x() < 0) break;
        moveBy(-1 * speed, 0);
        emit toChanged(type_);
        break;
    case Qt::Key_Right:
        if (pt.x() > scene_size_.width() - kScreenOffset) break;
        moveBy(speed, 0);
        emit toChanged(type_);
        break;
    case Qt::Key_Up:
        if (pt.y() < 0) break;
        moveBy(0, -1 * speed);
        emit toChanged(type_);
        break;
    case Qt::Key_Down:
        if (pt.y() > scene_size_.height() - kScreenOffset) break;
        moveBy(0, speed);
        emit toChanged(type_);
        break;
    default:;
    }

    update();
    QGraphicsItem::keyPressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScale::keyReleaseEvent(QKeyEvent *event) {
    update();
    QGraphicsItem::keyReleaseEvent(event);
}

//-------------------------------------------------------------------------
void QScreenScale::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton:
        left_pressed_ = true;
        emit toFocused(type_, index_);
        break;
    default:;
    }

    update();
    QGraphicsItem::mousePressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScale::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton: {
        left_pressed_ = false;

        QPointF pt = scenePos();
        if (pt.x() < 0) {
            pt.setX(0);
            setPos(pt);
        }
        if (pt.x() > scene_size_.width() - kScreenOffset) {
            pt.setX(scene_size_.width() - kScreenOffset);
            setPos(pt);
        }
        if (pt.y() < 0) {
            pt.setY(0);
            setPos(pt);
        }
        if (pt.y() > scene_size_.height() - kScreenOffset) {
            pt.setY(scene_size_.height() - kScreenOffset);
            setPos(pt);
        }

        emit toChanged(type_);
    } break;
    case Qt::RightButton: {
        QPoint pt;
        pt.setX(static_cast<int>(event->screenPos().x()));
        pt.setY(static_cast<int>(event->screenPos().y()));

        createPopupMenu(pt);
    } break;
    default:;
    }

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScale::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseMoveEvent(event);
}

//-------------------------------------------------------------------------
void QScreenScale::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    onSettingsShow();

    update();
    QGraphicsItem::mouseDoubleClickEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScale::wheelEvent(QGraphicsSceneWheelEvent *event) {
    QRectF r = rect();
    qreal h = rect().height();

    if (event->modifiers() & Qt::ShiftModifier) {
        QPointF pt = event->pos();
        if (pt.y() > (h / 2))  // bottom
            changeScaleBorder(false, event->delta());
        else  // top
            changeScaleBorder(true, event->delta());
    } else {
        if (event->delta() == 0) return;
        if (event->delta() > 0) {
            h += 20;
            if (h > kAxisYHeightMax) h = kAxisYHeightMax;
        } else {
            h -= 20;
            if (h < kAxisYHeightMin) h = kAxisYHeightMin;
        }
        r.setHeight(h);
        setRect(r);
    }

    drawDiag();
    drawScale();

    emit toChanged(type_);

    update();
    QGraphicsItem::wheelEvent(event);
}

//-------------------------------------------------------------------------
const QList<uint32_t> QScreenScale::diag_colors_ = {
    0xEF5350, 0xF44336, 0xE53935, 0xD32F2F, 0xC62828, 0xB71C1C, 0xEC407A, 0xE91E63, 0xD81B60, 0xC2185B, 0xAD1457,
    0x880E4F, 0xAB47BC, 0x9C27B0, 0x8E24AA, 0x7B1FA2, 0x6A1B9A, 0x4A148C, 0x7E57C2, 0x673AB7, 0x5E35B1, 0x512DA8,
    0x4527A0, 0x311B92, 0x5C6BC0, 0x3F51B5, 0x3949AB, 0x303F9F, 0x283593, 0x1A237E, 0x42A5F5, 0x2196F3, 0x1E88E5,
    0x1976D2, 0x1565C0, 0x0D47A1, 0x29B6F6, 0x03A9F4, 0x039BE5, 0x0288D1, 0x0277BD, 0x01579B, 0x26C6DA, 0x00BCD4,
    0x00ACC1, 0x0097A7, 0x00838F, 0x006064, 0x26A69A, 0x009688, 0x00897B, 0x00796B, 0x00695C, 0x004D40, 0x66BB6A,
    0x4CAF50, 0x43A047, 0x388E3C, 0x2E7D32, 0x1B5E20, 0x9CCC65, 0x8BC34A, 0x7CB342, 0x689F38, 0x558B2F, 0x33691E,
    0xD4E157, 0xCDDC39, 0xC0CA33, 0xAFB42B, 0x9E9D24, 0x827717, 0xFBC02D, 0xF9A825, 0xF57F17, 0xFFB300, 0xFFA000,
    0xFF8F00, 0xFF6F00, 0xFFA726, 0xFF9800, 0xFB8C00, 0xF57C00, 0xEF6C00, 0xE65100, 0xFF7043, 0xFF5722, 0xF4511E,
    0xE64A19, 0xD84315, 0xBF360C};

//-------------------------------------------------------------------------
