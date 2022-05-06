#include "qformscreen.h"

#include "GUI/WidgetWrappers.h"
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/Parameter_ifs.h"
#include "convtemplate/PrmBuffer_ifs.h"
#include "ui_qformscreen.h"
//
QString g_support_path;
//
template <typename Func2>
class CAction : public QAction {
   public:
    CAction(const QIcon &icon, const QString &text, const typename QtPrivate::FunctionPointer<Func2>::Object *parent,
            const QKeySequence &shortcut, Func2 slot)
        : QAction(icon, text, (QObject *)parent) {
        if (!shortcut.isEmpty()) setShortcut(shortcut);
        connect(this, &QAction::triggered, parent, slot);
    }

    CAction(const QString &text, const typename QtPrivate::FunctionPointer<Func2>::Object *parent,
            const QKeySequence &shortcut, Func2 slot)
        : QAction(text, (QObject *)parent) {
        if (!shortcut.isEmpty()) setShortcut(shortcut);
        connect(this, &QAction::triggered, parent, slot);
    }
};

//-------------------------------------------------------------------------
QFormScreen::QFormScreen(ExtensionManager *manager, PlotterContext_ifs *plotter_context, QWidget *parent)
    : manager_(manager), plotter_context_(plotter_context), QDialog(parent), ui_(new Ui::QFormScreen) {
    ui_->setupUi(this);

    margin_.left = kDiagramOffsetLeft;
    margin_.right = kDiagramOffsetLeft;

    setStyleSheet(
        "QWidget {background-color: #D2DCDF; alternate-background-color: #f6fafb; show-decoration-selected: 1;}");

    //
    program_data_path_ = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation);

    QSettings ini(QDir::currentPath() + ini_filename_eo_, QSettings::IniFormat);
    if (ini.isWritable()) g_support_path = QDir::currentPath();
    else {
        QString program_data_folder = QDir::toNativeSeparators(program_data_path_);
        if (!QDir(program_data_folder).exists()) QDir().mkdir(program_data_folder);

        g_support_path = program_data_folder;
    }

    Qt::WindowFlags flags = nullptr;
    flags |= Qt::Window;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    setWindowIcon(QIcon("://image_w"));

    time_width_.fromInt(30);

    TimeInterval current_interval;

    current_interval.end.fromInt(30);
    current_interval.bgn = current_interval.end - time_width_;

    // scene
    scene_ = new QScreenScene(manager, this);

    connect(this, &QFormScreen::toSceneChanged, scene_, &QGraphicsScene::advance);
    connect(scene_, &QScreenScene::toMarkerPlaced, this, &QFormScreen::onAddMarker);
    connect(scene_, &QScreenScene::toMenuCalled, this, &QFormScreen::onPopupMenuAdd);

    ui_->graphicsView->setScene(scene_);
    ui_->graphicsView->setMouseTracking(true);

    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    // Axis X
    axis_x_ = new QScreenAxisX(current_interval, lining_, margin_, this);

    connect(this, &QFormScreen::toSceneResized, axis_x_, &QScreenAxisX::onResize);
    connect(scene_, &QScreenScene::toLeftClicked, this, &QFormScreen::onAddMarkerAnchor);
    connect(scene_, &QScreenScene::toMouseWheeled, this, &QFormScreen::onZoom);
    connect(scene_, &QScreenScene::toMouseMoved, this, &QFormScreen::onUpdateMarkerFloat);
    connect(axis_x_, &QScreenAxisX::toHeight, this, &QFormScreen::onSetAxisXHeight);

    setupMenuBar();

    stats_.enabled = true;

    onAddMarker({15, 15});
}
#include <QDebug>

//-------------------------------------------------------------------------
QFormScreen::~QFormScreen() {
    qDebug() << "~QFormScreen()";
    delete plotter_context_;
    delete ui_;
}

//-------------------------------------------------------------------------
void QFormScreen::setupMenuBar() {
    auto *p_layout = reinterpret_cast<QHBoxLayout *>(layout());

    mn_file_ = new QMenu(tr("Экран"), this);
    toolbar_ = new QToolBar();

    QAction *action = new CAction(QIcon("://shot"), tr("Снимок"), this,  //
                                  Qt::CTRL + Qt::Key_S, &QFormScreen::onMakeShotAuto);
    mn_file_->addAction(action);
    toolbar_->addAction(action);

    action = new CAction(QIcon("://marker_off"), tr("видимость маркера"), this, {}, &QFormScreen::onHideMarker);

    toolbar_->addAction(action);

    mn_file_->addSeparator();

    mn_file_->addAction(new CAction(QIcon("://refresh"), tr("Обновить"), this,  //
                                    Qt::CTRL + Qt::Key_R, &QFormScreen::onRefreshScene));
    mn_file_->addAction(new CAction(QIcon("://greed"), tr("Сгруппировать"), this,  //
                                    Qt::CTRL + Qt::Key_G, &QFormScreen::onGroupScene));

    QIcon icon = QIcon();
    icon.addPixmap(QPixmap("://visible_on"), QIcon::Normal, QIcon::Off);
    icon.addPixmap(QPixmap("://visible_off"), QIcon::Normal, QIcon::On);
    action = new QAction(icon, tr("видимость шкал"), this);
    action->setCheckable(true);
    action->setChecked(is_axis_hidden_);
    connect(action, &QAction::triggered, this, &QFormScreen::onHideScale);
    mn_file_->addAction(action);

    mn_file_->addSeparator();

    mn_file_->addAction(new CAction(QIcon("://clear"), tr("Очистить"), this,  //
                                    Qt::CTRL + Qt::Key_Delete, &QFormScreen::onClearScene));
    mn_file_->addSeparator();
    mn_file_->addAction(new CAction(QIcon("://close"), tr("Выход"), this,  //
                                    Qt::ALT + Qt::Key_F4, &QFormScreen::onExit));

    // menu Settings
    act_settings_ = new CAction(QIcon(":/settings"), tr("Настройки"), this,  //
                                Qt::CTRL + Qt::Key_Q, &QFormScreen::onShowSettings);
    act_conf_save_ = new CAction(QIcon(":/save_conf"), tr("Сохранить конфигурацию"), this,  //
                                 {}, &QFormScreen::onSaveConf);

    mn_settings_ = new QMenu(tr("Настройки"), this);
    mn_settings_->addAction(act_settings_);
    mn_settings_->addAction(act_conf_save_);

    // main menu
    menubar_ = new QMenuBar(this);
    menubar_->addMenu(mn_file_);
    menubar_->addMenu(mn_settings_);
    p_layout->setMenuBar(menubar_);

    p_layout->insertWidget(0, toolbar_);

    // status & progress
    statusbar_ = new QStatusBar(this);

    p_layout->addWidget(statusbar_);
}
//-------------------------------------------------------------------------
QSizeF QFormScreen::getSceneSize() {
    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    return sz;
}

//-------------------------------------------------------------------------

QScreenScale *QFormScreen::addScale(QScreenScale *p_scale) {
    p_scale->setScale(&axis_x_->scale_);

    connect(p_scale, &QScreenScale::toRemoved, this, &QFormScreen::onRemoveItem);
    connect(p_scale, &QScreenScale::toTuned, this, &QFormScreen::onTuneItem);
    connect(p_scale, &QScreenScale::toAlign, this, &QFormScreen::onAlignItem);

    connect(p_scale, &QScreenScale::toChanged, this, &QFormScreen::onUpdateScene);
    connect(p_scale, &QScreenScale::toFocused, this, &QFormScreen::onUpdateFocus);
    connect(p_scale, &QScreenScale::toFocused, scene_, &QScreenScene::onSelectItem);

    connect(this, &QFormScreen::toIndexReduced, p_scale, &QScreenScale::onIndexReduce);
    connect(this, &QFormScreen::toSceneResized, p_scale, &QScreenScale::onResize);

    scales_.push_back(p_scale);

    scene_->addItem(p_scale);

    onResizeScene();

    return p_scale;
}

QScreenScale *QFormScreen::createScale(Reader_ifs *reader) {
    auto *p_scale =
        new QScreenScale(reader, scales_.count(), QSizeF(scene_->width(), scene_->height()), lining_, margin_, this);
    return addScale(p_scale);
}

//-------------------------------------------------------------------------
QScreenScale *QFormScreen::getScale(const int &index) {
    if (index >= scales_.count()) return nullptr;

    return scales_.at(index);
}
//-------------------------------------------------------------------------
void QFormScreen::onExit() {}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void QFormScreen::onRefresh(const RelativeTime &t, const bool zoomed) {
    axis_x_->current_borders_.end = t;
    axis_x_->current_borders_.bgn = t - time_width_;

    axis_x_->setInterval(axis_x_->current_borders_, zoomed);

    for (auto scale : scales_) scale->setTiming(axis_x_->current_borders_, {});

    onUpdateScene();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QFormScreen::onSetTimeVariables(const RelativeTime &t_width, const RelativeTime &t_step) { time_width_ = t_width; }
//-------------------------------------------------------------------------
void QFormScreen::onAddLabelExt(const RelativeTime &t_0, const int &y_0, const QString &txt,
                                const LineProperties &properties) {}
//-------------------------------------------------------------------------
void QFormScreen::onAddMakerExt(const RelativeTime &t_0) {
    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    QPointF pt;
    pt.setX(kScreenOffsetLeft);
    pt.setY(0);

    int index = markers_.count();

    auto *mrk = new QScreenMarker(axis_x_, index, pt, sz, lining_, this);

    mrk->p_scale_ = &axis_x_->scale_;

    connect(mrk, &QScreenMarker::toRemoved, this, &QFormScreen::onRemoveItem);
    connect(mrk, &QScreenMarker::to_changed, this, &QFormScreen::onUpdateScene);
    connect(mrk, &QScreenMarker::toFocused, this, &QFormScreen::onUpdateFocus);
    connect(mrk, &QScreenMarker::toFocused, scene_, &QScreenScene::onSelectItem);

    connect(this, &QFormScreen::toSceneResized, mrk, &QScreenMarker::onResize);
    connect(this, &QFormScreen::toIndexReduced, mrk, &QScreenMarker::onIndexReduce);

    // connect(axisX, &QScreenAxisX::to_changedInterval, mrk,
    // &QScreenMarker::on_setValid);

    markers_.push_back(mrk);
    scene_->addItem(mrk);

    emit mrk->toFocused(kSourceMarker, index);
    emit mrk->to_changed(kSourceMarker);
}
//-------------------------------------------------------------------------
void QFormScreen::onMessageShow(const QString &s) { QMessageBox::warning(this, tr("Внимание!"), s, QMessageBox::Ok); }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QFormScreen::saveShot(const QString &filename) {
    bool success = QFile::exists(filename);

    QFile image_file(filename);

    success = image_file.open(QIODevice::WriteOnly);

    scene_->clearSelection();  // Selections would also render to the file
    // scene->setSceneRect(scene->itemsBoundingRect()); // Re-shrink the scene to
    // it's bounding contents

    onRefreshScene();

    QImage image(scene_->sceneRect().size().toSize(),
                 QImage::Format_ARGB32);  //  Create the image with the exact size
    //  of the shrunk scene
    image.fill(Qt::transparent);  // Start all pixels transparent

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene_->render(&painter);
    success = image.save(image_file.fileName());

    image_file.close();

    return success;
}

//-------------------------------------------------------------------------
void QFormScreen::placeAxisX(QPainter *painter, const QScreenAxisX *axis) {
    QPointF pt(0, 0);

    QRectF rt = axis->img_->rect();

    painter->drawPixmap(pt, QPixmap::fromImage(*axis->img_), rt);
}

//-------------------------------------------------------------------------
void QFormScreen::placeMarker(QPainter *painter, const QScreenMarker *mrk) {
    QPointF pos = mrk->scenePos();
    QRectF rt = mrk->img_->rect();

    RelativeTime relative_time = mrk->relative_time_ - axis_x_->current_borders_.bgn;

    auto x = axis_x_->getXPos(relative_time);

    // x = pos.x();
    qreal scene_h = scene_->height();
    /*
        if (mrk->isVisible()) painter->drawPixmap(pos, QPixmap::fromImage(*mrk->img_), rt);
        else
            return;
    */
    // Markers
    if (!mrk->valid_) return;

    QPen pen(Qt::red);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    QFont ft = painter->font();
    ft.setPointSize(lining_.font_size);
    painter->setFont(ft);

    painter->drawLine(QPointF(x, 0), QPointF(x, scene_h));

    QPointF text_pos(x, scene_->height() - height_axis_x_ + 10);

    painter->setPen(mrk->color_);

    QString s = QString("%1")
                    .arg(mrk->t_.toDouble(), 0, 'f', 2, QChar('0'))
                    .replace(QLocale(QLocale::English).decimalPoint(), QLocale::system().decimalPoint());

    // TODO "axis_t_hms_" value is always true. is it normal?
    if (is_axis_t_hms_) s = secToHms(mrk->t_);

    painter->drawText(text_pos, s);

    placeMarkerValues(painter, x);
}

//-------------------------------------------------------------------------
void QFormScreen::placeTimeIndependentMarker(QPainter *painter, MarkerSimple &marker, RelativeTime distance) {
    if (!marker.enabled) return;

    qreal scene_h = scene_->height();

    marker.t = axis_x_->getRelativeTime(marker.x);

    QPen pen(Qt::black);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    QFont ft = painter->font();
    ft.setPointSize(lining_.font_size);
    painter->setFont(ft);

    painter->drawLine(QPointF(marker.x, 0), QPointF(marker.x, scene_h));

    QPointF text_pos(marker.x + 5, scene_->height() - height_axis_x_ - 10);

    double pixel_pow = log10(axis_x_->pixel_weight_);
    int prec = (pixel_pow < 0) ? static_cast<int>(fabs(pixel_pow)) + 1 : 0;

    RelativeTime t = marker.t + axis_x_->current_borders_.bgn;

    QString s = secToHms(t, prec);
    painter->drawText(text_pos, s);
    /*
        painter->drawText(
            text_pos,
            QString("%1: %2").arg(qlonglong(t.ls_integer)).arg(double(t.ms_fractional) / double(1ll << 32), 0, 'f', 3));
    */
    if (!(distance == RelativeTime{0, 0})) {
        QFontMetrics fm = painter->fontMetrics();
        auto offset = QPointF(0, qreal(fm.height()));
        painter->drawText(text_pos + offset, QString("Δt: %1").arg(distance.toDouble(), 0, 'f', prec));
    }

    if (axis_y_current_) placeMarkerValues(painter, marker.x);
}

//-------------------------------------------------------------------------
void QFormScreen::placeMarkerValues(QPainter *painter, int x) {
    /*
    int y = 0;
    QPointF pt;
    QFontMetrics fm = painter->fontMetrics();
    QList<int> occupied_dots;

    int pos_x = x - margin_.left;

    for (auto scl : scales_) {
        for (const auto &dots : scl->list_dots_) {
            if (dots.count() <= pos_x) continue;

            AxisXyDot dot = dots.at(pos_x);
            painter->setPen(QColor(scl->color_));
            if (dot.warning) painter->setPen(Qt::white);

            QPointF pt_d = scl->scenePos();  // Max Value
            pt.setX(x + 5);
            pt.setY(pt_d.y() + dot.y1 - kDiagramMargin + scl->img_shift_y_);

            QString s = formatValue(dot.val_max, scl->axis_y_.fmt_scale, scl->precision_scale_);
            QSize txt_size = fm.size(Qt::TextSingleLine, s);

            int txt_h = txt_size.height();
            y = static_cast<int>(pt.y() - txt_h + 2);  //
            bool occupied = true;
            int ct = 0;
            do {
                bool result = false;
                int first = 0;
                for (int j = y; j < y + txt_h; j++) {
                    if (occupied_dots.contains(j)) {
                        first = j;
                        result = true;
                        break;
                    }
                }
                if (result) y = first - txt_h;

                occupied = result;
                ct++;
                if (ct > 8) break;
            } while (occupied);
            for (int j = y; j < y + txt_h; j++) {
                occupied_dots.push_back(j);
            }  // qSort(occupiedDots);
            //
            QColor clr = Qt::white;
            if (dot.warning) clr = Qt::red;
            painter->fillRect(static_cast<int>(pt.x() - 2), y - 1, txt_size.width() + 3, txt_size.height() + 2,
                              clr);    // QColor(255, 255, 255, 192)
            pt.setY(y + txt_h - 2);    //
            painter->drawText(pt, s);  // Min Value
            // if(dot.ct > 1 && (dot.y0 != dot.y1)){
            if (dot.y0 != dot.y1) {
                // QString s = formatValue(dot.val_min, scl->axis_y_.fmt_scale, scl->precision_scale_);

                pt.setY(pt_d.y() + dot.y0 + txt_h + scl->img_shift_y_);
                // width   = fm.width(s) + 2;
                y = static_cast<int>(pt.y() - txt_h + 2);

                //
                occupied = true;
                ct = 0;
                do {
                    bool result = false;
                    int first = 0;
                    for (int j = y; j < y + txt_h; j++) {
                        if (occupied_dots.contains(j)) {
                            first = j;
                            result = true;
                            break;
                        }
                    }
                    if (result) y = first + txt_h;

                    occupied = result;
                    ct++;
                    if (ct > 8) break;
                } while (occupied);
                for (int j = y; j < y + txt_h; j++) {
                    occupied_dots.push_back(j);
                }
                // qSort(occupiedDots);
                //

                painter->fillRect(static_cast<int>(pt.x() - 1), y - 0, txt_size.width() + 3, txt_h + 2, clr);

                pt.setY(y + txt_h - 2);  //
                painter->drawText(pt, formatValue(dot.val_min, scl->axis_y_.fmt_scale, scl->precision_scale_));
            }
        }
    }
     */
}

//-------------------------------------------------------------------------

void QFormScreen::onResizeScene() {
    mark_a_.enabled = false;
    mark_f_.enabled = false;

    QRect rt = scene_->setRect(ui_->graphicsView->rect());

    int w = rt.width();
    int h = rt.height();

    /*for(auto d : scales){
        bool    f = false;
        qreal   y = d->pos().y();
        qreal   x = d->pos().x();

        if(d->pos().y() > h){
            y = h - d->rect().height() - DIAGRAM_MARGIN;
            f = true;
        }
        if(d->pos().x() > w){
            x = w - d->rect().width() - DIAGRAM_MARGIN;
            f = true;
        }

        if(f)
            d->setPos(x, y);

        d->setPos(x, y);
    }*/

    emit toSceneResized(w, h);

    onUpdateScene(kSourceMain);
}
//-------------------------------------------------------------------------
void QFormScreen::onZoom(const QPointF &pt, const int &delta) {
    Q_UNUSED(pt)

    double t = time_width_.toDouble();

    if (delta > 0) {
        if ((t + 1) <= 60) t++;
    } else {
        if ((t - 1) >= 1) t--;
    }

    time_width_.fromDouble(t);

    onRefresh(axis_x_->current_borders_.end, true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QFormScreen::onClearScene() {
    scene_->blockSignals(true);

    label_index_ = -1;

    scene_->blockSignals(true);
    for (auto mrk : markers_) {
        mrk->deleteLater();
        mrk->blockSignals(true);
        scene_->removeItem(mrk);
    }
    markers_.clear();

    /*for(auto lbl : labels){
        lbl->deleteLater();
        lbl->blockSignals(true);
        scene->removeItem(lbl);
    }
    labels.clear();*/

    for (auto d : scales_) {
        d->deleteLater();
        d->blockSignals(true);
        scene_->removeItem(d);
    }
    scales_.clear();

    onUpdateScene(kSourceMain);
    scene_->blockSignals(false);
}
//-------------------------------------------------------------------------
void QFormScreen::onRefreshScene() { onUpdateScene(kSourceMain); }
//-------------------------------------------------------------------------
void QFormScreen::onGroupScene() {
    int x = kAxisYDiagInterval;
    int y = kScreenOffsetTop;

    int dx = kAxisYWidth;  // + AXIS_Y_DIAG_INTERVAL;
    int dy = kAxisYDiagInterval;

    int h = int(scene_->height()) - kScreenOffsetTop;  // - SCREEN_OFFSET_BOTTOM;

    int c = 0;

    for (auto d : scales_) {
        d->setPos(x, y);

        y += int(d->rect().height() + dy);

        if (y > (h - kAxisYDiagInterval)) {
            c++;
            x += dx;
            y = kScreenOffsetLeft + int(0.5 * kAxisYHeightDefault * (c & 0x1));
        }
    }

    onUpdateScene(kSourceMain);
}
//-------------------------------------------------------------------------
void QFormScreen::onUpdateScene(const int &src) {
    /*switch (src) {
    case SOURCE_MAIN:
        //markA.enabled = false;
        //markF.enabled = false;
        break;
    case SOURCE_AXIS_X:
        //markA.enabled = false;
        //markF.enabled = false;
        break;
    default:
        ;
    }*/

    bool warning = false;
    qreal scene_w = scene_->width();
    qreal scene_h = scene_->height();
    QPixmap pm_scene(static_cast<int>(scene_w), static_cast<int>(scene_h));
    pm_scene.fill(Qt::white);
    auto *painter = new QPainter(&pm_scene);

    QFont ft = painter->font();
    ft.setPointSize(lining_.font_size);
    painter->setFont(ft);

    // Axis X (Time)
    placeAxisX(painter, axis_x_);

    // Diagrams & Scales
    for (auto d : scales_) {
        d->placePoints(painter);
        d->placeScale(painter, is_axis_hidden_);
        if (stats_.enabled) d->placeStat(painter);
        warning |= d->warning_;
    }

    // Markers
    for (auto mrk : markers_) {
        placeMarker(painter, mrk);
    }

    // Simple Markers
    if (mark_f_.enabled) {
        placeTimeIndependentMarker(painter, mark_a_, {0, 0});
        placeTimeIndependentMarker(painter, mark_f_, mark_a_.enabled ? (mark_f_.t - mark_a_.t) : RelativeTime{0, 0});
    }

    delete painter;

    scene_->setBackgroundBrush(pm_scene);

    if (warning & saving_.auto_save) onMakeShotAuto();

    emit toSceneChanged();
}
//-------------------------------------------------------------------------
void QFormScreen::onUpdateFocus(const int &src, const int &index_last) {
    QString s = "";

    switch (src) {
    case kSourceScale:
        // diagIndex   = indexLast;
        // s = tr("Фокус: %1 (%2)").arg(diagrams.at(indexLast)->sName).arg(indexLast
        // + 1);
        break;
    case kSourceLabel:
        label_index_ = index_last;
        s = tr("Фокус: Надпись %1").arg(index_last + 1);
        break;
    case kSourceMarker:
        marker_index_ = index_last;
        s = tr("Фокус: Маркер %1").arg(index_last + 1);
        break;
    case kSourceBorder:
        if (index_last) s = tr("Фокус: Правая граница области отображения графиков");
        else
            s = tr("Фокус: Левая граница области отображения графиков");
        break;
    default:
        s = tr("...");
    }

    // statusbar->showMessage(s);
}
//-------------------------------------------------------------------------
void QFormScreen::onPopupMenuAdd(const QPointF &pt) {
    auto *act_add_marker = new QAction(tr("Добавить маркер"), this);
    act_add_marker->setData(pt);
    connect(act_add_marker, &QAction::triggered, this, &QFormScreen::onAddMarkerFromMenu);

    auto *act_add_label = new QAction(tr("Добавить надпись"), this);
    act_add_label->setData(pt);
    connect(act_add_label, &QAction::triggered, this, &QFormScreen::onAddLabelFromMenu);

    auto *menu = new QMenu(this);
    menu->addAction(act_add_marker);
    menu->addAction(act_add_label);
    menu->exec(QCursor::pos());

    // menu->deleteLater();
}
//-------------------------------------------------------------------------
void QFormScreen::onAddMarker(const QPointF &pt) {
    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    int index = markers_.count();
    auto *mrk = new QScreenMarker(axis_x_, index, pt, sz, lining_, this);

    mrk->p_scale_ = &axis_x_->scale_;

    connect(mrk, &QScreenMarker::toRemoved, this, &QFormScreen::onRemoveItem);
    connect(mrk, &QScreenMarker::to_changed, this, &QFormScreen::onUpdateScene);
    connect(mrk, &QScreenMarker::toFocused, this, &QFormScreen::onUpdateFocus);
    connect(mrk, &QScreenMarker::toFocused, scene_, &QScreenScene::onSelectItem);

    connect(this, &QFormScreen::toSceneResized, mrk, &QScreenMarker::onResize);
    connect(this, &QFormScreen::toIndexReduced, mrk, &QScreenMarker::onIndexReduce);

    // connect(axisX, &QScreenAxisX::to_changedInterval, mrk,
    // &QScreenMarker::on_setValid);

    markers_.push_back(mrk);
    scene_->addItem(mrk);

    emit mrk->toFocused(kSourceMarker, index);
    emit mrk->to_changed(kSourceMarker);
}
//-------------------------------------------------------------------------
void QFormScreen::onAddMarkerFromMenu() {
    auto *act_src = qobject_cast<QAction *>(sender());

    auto pt = qvariant_cast<QPointF>(act_src->data());

    onAddMarker(pt);
}
//-------------------------------------------------------------------------
void QFormScreen::onAddLabel(const QPointF &pt) {}
//-------------------------------------------------------------------------
void QFormScreen::onAddLabelFromMenu() {
    auto *act_src = qobject_cast<QAction *>(sender());

    auto pt = qvariant_cast<QPointF>(act_src->data());

    onAddLabel(pt);
}
//-------------------------------------------------------------------------
void QFormScreen::onAddMarkerAnchor(const QPointF &pt) {
    if ((pt.x() < margin_.left) || (pt.x() > scene_->width() - margin_.right)) {
        mark_a_.enabled = false;
        return;
    }

    mark_a_.enabled = true;
    mark_a_.x = pt.x();
    mark_a_.y = pt.y();  // not used
    mark_a_.t = axis_x_->getRelativeTime(pt.x());

    onUpdateScene(kSourceMarkerAnchor);
}

//-------------------------------------------------------------------------
void QFormScreen::onUpdateMarkerFloat(const QPointF &pt) {
    if ((pt.x() < margin_.left) || (pt.x() > scene_->width() - margin_.right)) {
        mark_f_.enabled = false;
        return;
    }

    mark_f_.enabled = true;
    mark_f_.x = pt.x();
    mark_f_.y = pt.y();
    mark_f_.t = axis_x_->getRelativeTime(pt.x());

    onUpdateScene(kSourceMarkerFloat);
}
//-------------------------------------------------------------------------
void QFormScreen::onChangeSettings(const QString &title, const SettingsCommon &stx_cmn) {
    title_ = title;

    // setWindowTitle(getTitle());

    time_width_ = stx_cmn.timing.width;

    margin_ = stx_cmn.margin;
    lining_ = stx_cmn.lining;
    saving_ = stx_cmn.saving;

    // scales
    axis_x_->current_borders_.bgn = axis_x_->current_borders_.end - time_width_;

    axis_x_->setMargin(margin_);
    axis_x_->setInterval(axis_x_->current_borders_, true);
    for (auto scale : scales_) {
        scale->setMargins(margin_);
        scale->setTiming(axis_x_->current_borders_, {});
    }
}
//-------------------------------------------------------------------------
void QFormScreen::onRemoveItem(const int &src, const int &index) {
    scene_->blockSignals(true);

    switch (src) {
    case kSourceScale: {
        QList<int> list;
        list.push_back(index);
        if (index < 0) {
            for (auto d : scales_) {
                list.push_back(d->getIndex());
            }
        } else {
            for (auto d : scales_) {
                if (d->isSelected()) {
                    if (!list.contains(d->getIndex())) list.push_back(d->getIndex());
                }
            }
        }

        std::sort(list.begin(), list.end(), std::greater<int>());
        for (int j : list) {
            QScreenScale *p = scales_.takeAt(j);
            p->blockSignals(true);
            scene_->removeItem(p);

            emit toIndexReduced(src, p->getIndex());

            p->deleteLater();
        }

    } break;
    case kSourceMarker: {
        QList<int> list;
        list.push_back(index);
        if (index < 0) {
            for (auto mrk : markers_) {
                list.push_back(mrk->getIndex());
            }
        } else {
            for (auto mrk : markers_) {
                if (mrk->isSelected()) {
                    if (!list.contains(mrk->getIndex())) list.push_back(mrk->getIndex());
                }
            }
        }

        std::sort(list.begin(), list.end(), std::greater<int>());
        for (int j : list) {
            QScreenMarker *p = markers_.takeAt(j);
            p->blockSignals(true);
            scene_->removeItem(p);

            emit toIndexReduced(src, p->getIndex());

            p->deleteLater();
        }

    } break;
        /*case SOURCE_LABEL:{
            QList<int>  list;
            list.push_back(index);
            if(index < 0){
                for(auto lbl : labels){
                    list.push_back(lbl->getIndex());
                }
            }
            else {
                for(auto lbl : labels){
                    if(lbl->isSelected()){
                        if(!list.contains(lbl->getIndex()))
                            list.push_back(lbl->getIndex());
                    }
                }
            }

            std::sort(list.begin(), list.end(), std::greater<int>());
            for(int j : list){
                QDiagLabel *p = labels.takeAt(j);
                p->blockSignals(true);
                scene->removeItem(p);

                emit toIndexReduced(src, p->getIndex());

                p->deleteLater();
            }

            labelIndex = labels.count() - 1;
        }
        break;*/
    default:;
    }

    scene_->blockSignals(false);
}
//-------------------------------------------------------------------------
void QFormScreen::onTuneItem(const int &src, const int &index) {}
//-------------------------------------------------------------------------
void QFormScreen::onAlignItem(const int &src, const int &index, const int &val) {
    if (src != kSourceScale) return;

    if (index < 0) return;

    QRectF rt;  // = scales.at(index)->rect();

    qreal x_min, x_max, y_min, y_max;
    int ct = 0;
    for (auto d : scales_) {
        if (!d->isSelected()) continue;

        if (!ct) {
            x_min = d->pos().x();
            x_max = x_min;
            y_min = d->pos().y();
            y_max = y_min + d->rect().height();
        } else {
            x_min = std::min(x_min, d->pos().x());
            x_max = std::max(x_max, d->pos().x());
            y_min = std::min(y_min, d->pos().y());
            y_max = std::max(y_max, (d->pos().y() + d->rect().height()));
        }
        ct++;

        if (d->getIndex() == index) {
            rt = d->rect();
        }
    }

    for (auto d : scales_) {
        if (!d->isSelected()) continue;

        switch (val) {
        case 0:
            d->setPos(d->pos().x(), y_min);
            break;
        case 1:
            d->setPos(d->pos().x(), y_max - d->rect().height());
            break;
        case 2:
            d->setPos(x_min, d->pos().y());
            break;
        case 3:
            d->setPos(x_max, d->pos().y());
            break;
        default:
            if (d->getIndex() != index) {
                d->setRect(d->rect().x(), d->rect().y(), d->rect().width(), rt.height());
                d->formPointsImage();
                d->formScaleImage();
            }
        }
    }

    onUpdateScene(kSourceMain);
}
//-------------------------------------------------------------------------
void QFormScreen::onSetAxisXHeight(const int &h) { height_axis_x_ = h; }
//-------------------------------------------------------------------------
void QFormScreen::onHideScale(bool checked) { is_axis_hidden_ = checked; }
//-------------------------------------------------------------------------
void QFormScreen::onHideMarker(bool checked) {
    mark_f_.enabled = false;
    mark_a_.enabled = false;
}
//-------------------------------------------------------------------------

void QFormScreen::onShowSettings() {
    auto *fs = new QFormScreenSettings(this);

    SettingsCommon stx;
    stx.lining = lining_;
    stx.margin = margin_;
    stx.timing = time_;
    stx.saving = saving_;

    fs->setCommon(title_, stx);

    // TODO: scales settings

    connect(fs, &QFormScreenSettings::to_confirmed, this, &QFormScreen::onChangeSettings);

    fs->setActiveTab(0);
    fs->show();
}
//-------------------------------------------------------------------------
void QFormScreen::onSaveConf() {}
//-------------------------------------------------------------------------
void QFormScreen::onReport(const QString &s) { statusbar_->showMessage(s); }
//-------------------------------------------------------------------------
void QFormScreen::onMakeShotManual() {
    QSettings ini(g_support_path + ini_filename_eo_, QSettings::IniFormat);
    QString path = ini.value(ini_image_save_path_, g_support_path).toString();

    QFileDialog dlg(this, tr("Сохранение в графический файл"), path);
    dlg.setDefaultSuffix(".png");
    connect(&dlg, &QFileDialog::filterSelected, this, &QFormScreen::onSaveImageFilterChange);

    dlg.setNameFilters(QStringList() << "Portable Network Graphics (*.png)"
                                     << "Joint Photographic Experts Group (*.jpg)"
                                     << "Bitmap (*.bmp)");
    dlg.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);

    int result = dlg.exec();
    if (result != QFileDialog::Accepted) return;

    QString filename = dlg.selectedFiles().first();

    bool success = saveShot(filename);
    Q_UNUSED(success)

    ini.setValue(ini_image_save_path_, QFileInfo(filename).path());
    ini.sync();
}
//-------------------------------------------------------------------------
void QFormScreen::onMakeShotAuto() {
    QString filename =
        QString("%1/%3.png").arg(g_support_path).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"));

    filename.replace(" ", "_");

    bool success = saveShot(filename);
    Q_UNUSED(success)
}
//-------------------------------------------------------------------------
void QFormScreen::onSaveImageFilterChange(const QString &filter) {
    auto *dlg = qobject_cast<QFileDialog *>(sender());

    if (filter.contains(".png")) {
        dlg->setDefaultSuffix(".png");
        return;
    }
    if (filter.contains(".jpg") || filter.contains(".jpeg")) {
        dlg->setDefaultSuffix(".jpg");
        return;
    }
    if (filter.contains(".bmp")) {
        dlg->setDefaultSuffix(".bmp");
        return;
    }
    if (filter.contains(".svg")) {
        dlg->setDefaultSuffix(".svg");
        return;
    }
}
//-------------------------------------------------------------------------
void QFormScreen::closeEvent(QCloseEvent *event) { event->accept(); }
//-------------------------------------------------------------------------
void QFormScreen::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)

    onResizeScene();
}

//-------------------------------------------------------------------------
