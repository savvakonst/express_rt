#include "qformscreen.h"

#include <QIcon>

#include "ui_qformscreen.h"

//
QString g_support_path;
//
template <typename Func2>
class ActionC : public QAction {
   public:
    ActionC(const QIcon &icon, const QString &text, const typename QtPrivate::FunctionPointer<Func2>::Object *parent,
            const QKeySequence &shortcut, Func2 slot)
        : QAction(icon, text, (QObject *)parent) {
        if (!shortcut.isEmpty()) setShortcut(shortcut);
        connect(this, &QAction::triggered, parent, slot);
    }
    ActionC(const QString &text, const typename QtPrivate::FunctionPointer<Func2>::Object *parent,
            const QKeySequence &shortcut, Func2 slot)
        : QAction(text, (QObject *)parent) {
        if (!shortcut.isEmpty()) setShortcut(shortcut);
        connect(this, &QAction::triggered, parent, slot);
    }
};
//-------------------------------------------------------------------------
QFormScreen::QFormScreen(QWidget *parent) : QDialog(parent), ui_(new Ui::QFormScreen) {
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

    current_time_.end.fromInt(30);
    current_time_.bgn = current_time_.end - time_width_;

    // scene
    scene_ = new QScreenScene(this);

    connect(this, &QFormScreen::toSceneChanged, scene_, &QGraphicsScene::advance);
    connect(scene_, &QScreenScene::toMarkerPlaced, this, &QFormScreen::onAddMarker);
    connect(scene_, &QScreenScene::toMenuCalled, this, &QFormScreen::onPopupMenuAdd);

    ui_->graphicsView->setScene(scene_);
    ui_->graphicsView->setMouseTracking(true);

    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    // Axis X
    axis_x_ = new QScreenAxisX(current_time_, lining_, margin_, this);

    connect(this, &QFormScreen::toSceneResized, axis_x_, &QScreenAxisX::onResize);
    // connect(scene, &QScreenScene::to_leftGestured, axisX, &QScreenAxisX::on_zoom);
    connect(scene_, &QScreenScene::toLeftClicked, this, &QFormScreen::onAddMarkerAnchor);
    // connect(scene, &QScreenScene::to_rightGestured, axisX, &QScreenAxisX::on_move);
    connect(scene_, &QScreenScene::to_mouseWheeled, this, &QFormScreen::onZoom);
    connect(scene_, &QScreenScene::to_paused, this, &QFormScreen::onPause);
    connect(scene_, &QScreenScene::to_mouseMoved, this, &QFormScreen::onUpdateMarkerFloat);

    // connect(axisX, &QScreenAxisX::toChanged, this, &QFormScreen::onUpdateScene);
    connect(axis_x_, &QScreenAxisX::to_height, this, &QFormScreen::onSetAxisXHeight);

    auto *p_layout = reinterpret_cast<QHBoxLayout *>(layout());

    auto image_path = QCoreApplication::applicationDirPath() + "/png/common/play.png";
    QIcon run_icon = QIcon::fromTheme("run", QIcon(image_path));
    auto *act_run = new QAction(run_icon, tr("Ru&n"), this);
    // connect(this, &QAction::triggered, this, &RunAction::run);
    setStatusTip(tr("run"));

    // menu File
    act_shot_ = new ActionC(QIcon("://shot"), tr("Снимок"), this,  //
                            Qt::CTRL + Qt::Key_S, &QFormScreen::onMakeShotAuto);

    act_shot_as_ = new ActionC(QIcon("://shot"), tr("Сохранить снимок как ..."), this,  //
                               Qt::CTRL + Qt::SHIFT + Qt::Key_S, &QFormScreen::onMakeShotManual);

    act_refresh_ = new ActionC(QIcon("://refresh"), tr("Обновить"), this,  //
                               Qt::CTRL + Qt::Key_R, &QFormScreen::onRefreshScene);

    act_group_up_ = new ActionC(QIcon("://greed"), tr("Сгруппировать"), this,  //
                                Qt::CTRL + Qt::Key_G, &QFormScreen::onGroupupScene);

    act_scale_hide_ = new QAction(QIcon("://hidden"), tr("Скрыть все шкалы"), this);
    act_scale_hide_->setCheckable(true);
    act_scale_hide_->setChecked(is_axis_hidden_);
    if (is_axis_hidden_) act_scale_hide_->setText(tr("Показать шкалы"));
    connect(act_scale_hide_, &QAction::triggered, this, &QFormScreen::onHideScale);

    act_clear_ = new ActionC(QIcon("://clear"), tr("Очистить"), this,  //
                             Qt::CTRL + Qt::Key_Delete, &QFormScreen::onClearScene);

    act_exit_ = new ActionC(QIcon("://close"), tr("Выход"), this,  //
                            Qt::ALT + Qt::Key_F4, &QFormScreen::onExit);

    mn_file_ = new QMenu(tr("Экран"), this);
    mn_file_->addAction(act_shot_);
    mn_file_->addSeparator();
    mn_file_->addAction(act_refresh_);
    mn_file_->addAction(act_group_up_);
    mn_file_->addAction(act_scale_hide_);
    mn_file_->addSeparator();
    mn_file_->addAction(act_clear_);
    mn_file_->addSeparator();
    mn_file_->addAction(act_exit_);
    mn_file_->addAction(act_run);

    // menu Settings
    {
        act_settings_ = new QAction(QIcon(":/settings"), tr("Настройки"), this);
        act_settings_->setShortcut(Qt::CTRL + Qt::Key_Q);
        connect(act_settings_, &QAction::triggered, this, &QFormScreen::onShowSettings);

        act_conf_save_ = new QAction(QIcon(":/save_conf"), tr("Сохранить конфигурацию"), this);
        connect(act_conf_save_, &QAction::triggered, this, &QFormScreen::onSaveConf);

        mn_settings_ = new QMenu(tr("Настройки"), this);
        mn_settings_->addAction(act_settings_);
        mn_settings_->addAction(act_conf_save_);
    }

    // menu help
    {
        act_help_ = new QAction(tr("Справка"), this);
        act_help_->setShortcut(Qt::Key_F1);
        connect(act_help_, &QAction::triggered, this, &QFormScreen::onHelp);

        mn_help_ = new QMenu(tr("Справка"), this);
        mn_help_->addAction(act_help_);
    }

    // main menu
    menubar_ = new QMenuBar(this);
    menubar_->addMenu(mn_file_);
    menubar_->addMenu(mn_settings_);
    menubar_->addMenu(mn_help_);
    p_layout->setMenuBar(menubar_);

    // toolbar
    toolbar_ = new QToolBar();
    toolbar_->addAction(act_shot_);
    toolbar_->addAction(act_run);
    // toolbar->addAction(actShotAs);

    p_layout->insertWidget(0, toolbar_);

    // status & progress
    statusbar_ = new QStatusBar(this);  // Progress Bar

    progress_ = new QProgressBar(statusbar_);
    progress_->setAlignment(Qt::AlignRight);
    progress_->setMaximumWidth(200);
    progress_->setMaximum(100);
    progress_->setVisible(false);

    statusbar_->addPermanentWidget(progress_);
    // statusbar_->showMessage(getTitle());
    p_layout->addWidget(statusbar_);

    // show();
}
#include <QDebug>
//-------------------------------------------------------------------------
QFormScreen::~QFormScreen() {
    qDebug() << "~QFormScreen()";

    delete ui_;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QFormScreen::setTitle(const QString &title) {
    title_ = title;
    // setWindowTitle(title_);

    emit to_titleChanged();
}

//-------------------------------------------------------------------------
QSizeF QFormScreen::getSceneSize() {
    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    return sz;
}

//-------------------------------------------------------------------------
void QFormScreen::setSettings(const LineProperties &dstx) {
    lining_ = dstx;

    act_scale_hide_->setChecked(is_axis_hidden_);
    if (is_axis_hidden_) act_scale_hide_->setText(tr("Показать шкалы"));
    else
        act_scale_hide_->setText(tr("Скрыть шкалы"));
}

//-------------------------------------------------------------------------
void QFormScreen::setInterval(const TimeInterval &ti0) {
    /*axisX->setInterval(ti0);
    for(auto scale_ : scales){
        scale_->onSetInterval(ti);
    }*/
}

//-------------------------------------------------------------------------

QScreenScale *QFormScreen::addScale(Reader_ifs *reader) {
    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    int index = scales_.count();
    auto *scl = new QScreenScale(reader, index, sz, lining_, margin_, this);

    // scl->setParameter(name, prm);
    scl->setScale(&axis_x_->scale_);
    // scl->setToolTip(prm->info().join("\n"));

    connect(scl, &QScreenScale::toRemoved, this, &QFormScreen::onRemoveItem);
    connect(scl, &QScreenScale::toTuned, this, &QFormScreen::onTuneItem);
    connect(scl, &QScreenScale::toAlign, this, &QFormScreen::onAlignItem);
    connect(scl, &QScreenScale::toProgressed, this, &QFormScreen::onProgress);

    connect(scl, &QScreenScale::toChanged, this, &QFormScreen::onUpdateScene);
    connect(scl, &QScreenScale::toFocused, this, &QFormScreen::onUpdateFocus);
    connect(scl, &QScreenScale::toFocused, scene_, &QScreenScene::onSelectItem);

    connect(this, &QFormScreen::toIndexReduced, scl, &QScreenScale::onIndexReduce);
    connect(this, &QFormScreen::toSceneResized, scl, &QScreenScale::onResize);
    connect(this, &QFormScreen::toPaused, scl, &QScreenScale::onSetPause);

    // connect(axisX, &QScreenAxisX::to_changedInterval, scl,
    // &QScreenScale::onSetInterval); connect(scl, &QScreenScale::to_definedTime,
    // axisX, &QScreenAxisX::on_getXByTime); connect(axisX,
    // &QScreenAxisX::to_definedX, scl, &QScreenScale::onSetX);

    scales_.push_back(scl);
    scale_index_ = scales_.count() - 1;

    scene_->addItem(scl);

    onResizeScene();

    return scl;
}

QScreenScale *QFormScreen::addScale() {
    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    int index = scales_.count();
    auto *scl = new QScreenScale(index, sz, lining_, margin_, this);

    // scl->setParameter(name, prm);
    scl->setScale(&axis_x_->scale_);
    // scl->setToolTip(prm->info().join("\n"));

    connect(scl, &QScreenScale::toRemoved, this, &QFormScreen::onRemoveItem);
    connect(scl, &QScreenScale::toTuned, this, &QFormScreen::onTuneItem);
    connect(scl, &QScreenScale::toAlign, this, &QFormScreen::onAlignItem);
    connect(scl, &QScreenScale::toProgressed, this, &QFormScreen::onProgress);

    connect(scl, &QScreenScale::toChanged, this, &QFormScreen::onUpdateScene);
    connect(scl, &QScreenScale::toFocused, this, &QFormScreen::onUpdateFocus);
    connect(scl, &QScreenScale::toFocused, scene_, &QScreenScene::onSelectItem);

    connect(this, &QFormScreen::toIndexReduced, scl, &QScreenScale::onIndexReduce);
    connect(this, &QFormScreen::toSceneResized, scl, &QScreenScale::onResize);
    connect(this, &QFormScreen::toPaused, scl, &QScreenScale::onSetPause);

    // connect(axisX, &QScreenAxisX::to_changedInterval, scl,
    // &QScreenScale::onSetInterval); connect(scl, &QScreenScale::to_definedTime,
    // axisX, &QScreenAxisX::on_getXByTime); connect(axisX,
    // &QScreenAxisX::to_definedX, scl, &QScreenScale::onSetX);

    scales_.push_back(scl);
    scale_index_ = scales_.count() - 1;

    scene_->addItem(scl);

    onResizeScene();

    return scl;
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
    current_time_.end = t;
    current_time_.bgn = t - time_width_;

    axis_x_->setInterval(current_time_, zoomed);
    for (auto scale : scales_) {
        scale->setTiming(current_time_, {});
    }

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

    auto *mrk = new QScreenMarker(index, pt, sz, lining_, this);

    mrk->setTime(t_0);
    mrk->pScale_ = &axis_x_->scale_;

    connect(mrk, &QScreenMarker::to_removed, this, &QFormScreen::onRemoveItem);
    connect(mrk, &QScreenMarker::to_changed, this, &QFormScreen::onUpdateScene);
    connect(mrk, &QScreenMarker::to_focused, this, &QFormScreen::onUpdateFocus);
    connect(mrk, &QScreenMarker::to_focused, scene_, &QScreenScene::onSelectItem);

    connect(this, &QFormScreen::toSceneResized, mrk, &QScreenMarker::on_resize);
    connect(this, &QFormScreen::toIndexReduced, mrk, &QScreenMarker::on_indexReduce);

    // connect(axisX, &QScreenAxisX::to_changedInterval, mrk,
    // &QScreenMarker::on_setValid);

    markers_.push_back(mrk);
    scene_->addItem(mrk);

    emit mrk->to_focused(kSourceMarker, index);
    emit mrk->to_changed(kSourceMarker);
}
//-------------------------------------------------------------------------
void QFormScreen::onMessageShow(const QString &s) { QMessageBox::warning(this, tr("Внимание!"), s, QMessageBox::Ok); }
//-------------------------------------------------------------------------
void QFormScreen::onHelp() { emit to_help(); }
//-------------------------------------------------------------------------
bool QFormScreen::saveShot(const QString &filename) {
    bool success = QFile().exists(filename);

    QFile imageFile(filename);

    success = imageFile.open(QIODevice::WriteOnly);

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
    success = image.save(imageFile.fileName());

    imageFile.close();

    return success;
}

//-------------------------------------------------------------------------
void QFormScreen::placeAxisX(QPainter *painter, const QScreenAxisX *axis) {
    QPointF pt(0, 0);

    QRectF rt = axis->img_->rect();

    painter->drawPixmap(pt, QPixmap::fromImage(*axis->img_), rt);
}

//-------------------------------------------------------------------------
void QFormScreen::placeDiag(QPainter *painter, const QScreenScale *scl) const {
    QPointF pt = scl->pos();
    pt.setX(0);

    int y = static_cast<int>(pt.y());
    y -= kDiagramMargin;
    y += scl->img_shift_y_;
    pt.setY(y);
    int x = static_cast<int>(pt.x());
    x += margin_.left;
    pt.setX(x);

    QRectF rt = scl->img_diag_->rect();

    painter->drawPixmap(pt, QPixmap::fromImage(*scl->img_diag_), rt);
}

//-------------------------------------------------------------------------
void QFormScreen::placeScale(QPainter *painter, const QScreenScale *scl) {
    QPointF pt = scl->scenePos();
    QRectF rt = scl->img_scale_->rect();

    QFontMetrics fm = painter->fontMetrics();
    int txtW = 0;
    int txtH = fm.height();
    QRect txtRt;

    if (!is_axis_hidden_) {
        painter->drawPixmap(pt, QPixmap::fromImage(*scl->img_scale_), rt);

        pt.setY(pt.y() - 5);
        painter->setPen(QColor(scl->color_));

        for (auto co : scl->cutoffs_) {
            if (!co.enabled) continue;

            QString s = formatValue(co.val, scl->axis_y_.fmt_scale, scl->precision_scale_);

            txtW = fm.width(s) + 14;
            txtRt = fm.boundingRect((pt.x() + 9), (pt.y() + co.pos), txtW, txtH + 1, 0, s);

            painter->fillRect(txtRt, QBrush(Qt::white));
            painter->drawText((pt.x() + 10), (pt.y() + co.pos + (txtH * 0.8)), s);
        }

        painter->drawText(pt, scl->s_label_);
    }

    // TODO: "scl->axis_y_.greed" variable is used only once .
    if (!scl->axis_y_.greed || scl->cutoffs_.empty()) return;

    // QPen    pen(Qt::lightGray);
    QPen pen(QColor(0x33, 0x33, 0x33, 0x5F));
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    for (auto co : scl->cutoffs_) {
        qreal y = pt.y() + co.pos + kDiagramMargin;
        painter->drawLine(QPointF(margin_.left - kDiagramMargin, y), QPointF(width() - margin_.right, y));
    }
}
//-------------------------------------------------------------------------
void QFormScreen::placeStat(QPainter *painter, const QScreenScale *scl) {
    if (!stats_.enabled) return;

    if (scl->list_dots_.isEmpty()) return;

    QRect rt = scene_->setRect(ui_->graphicsView->rect());
    QPointF pt = scl->scenePos();

    pt.setX(rt.width() - margin_.right + kDiagramMargin);

    QFontMetrics fm = painter->fontMetrics();

    /*QPen    pen;
    pen.setColor(scl.d);
    pen.setWidth(5);
    painter->setPen(pen);*/

    scl->list_dots_.last();

    QString s;
    QColor clrBack = Qt::white;
    QColor clrText = scl->color_;
    if (scl->warning_) {
        clrBack = Qt::red;
        clrText = Qt::white;
    }
    painter->setPen(clrText);

    for (const auto &dots : scl->list_dots_) {
        if (dots.last().val_max != dots.last().val_min) {
            s = QString("max: %1").arg(dots.last().val_max);

            QSize txtSize = fm.size(Qt::TextSingleLine, s);

            painter->fillRect(static_cast<int>(pt.x() - 2), pt.y() - txtSize.height() + 1, txtSize.width() + 5,
                              txtSize.height() + 2, clrBack);
            painter->drawText(pt, s);
            pt.setY(pt.y() + fm.size(Qt::TextSingleLine, s).height() + 2);

            s = QString("min: %1").arg(dots.last().val_max);

            txtSize = fm.size(Qt::TextSingleLine, s);

            painter->fillRect(static_cast<int>(pt.x() - 2), pt.y() - txtSize.height() + 1, txtSize.width() + 5,
                              txtSize.height() + 2, clrBack);
            painter->drawText(pt, s);
            pt.setY(pt.y() + fm.size(Qt::TextSingleLine, s).height() + 2);
        } else {
            s = QString("val: %1").arg(dots.last().val_max);

            painter->drawText(pt, s);
            pt.setY(pt.y() + fm.size(Qt::TextSingleLine, s).height() + 2);
        }
    }

    /*int fieldsCount = 1;
    for (int j = 1; j < static_cast<int>(sizeof(stats_)); j++) {
        if (stats_.b[j])
            fieldsCount++;
    }

    qreal step = (dstx_.font_size + 8);
    qreal x = scene_->width() - dstx_.pos.right + 6;
    qreal y = (scl->getIndex()) * (fieldsCount * step) + step;

    painter->setPen(QColor(scl->color_));
    painter->drawText(QPointF(x, y), scl->s_label_);
    y += step;


    if (stats_.current) {
        QString curValue = "";

        for (int j = 0; j < scl->list_dots_.count(); j++) {
            if (j)
                curValue += "";

            QVector<AxisXReference> refs = scl->list_refs_.at(j);
            QVector<AxisXyDot> dots = scl->list_dots_.at(j);

            do {
                AxisXReference ref = refs.at(mark_f_.x);
                if (ref.index < 0)
                    break;
                if (ref.index >= dots.count())
                    break;

                AxisXyDot dot = dots.at(ref.index);

                curValue += QString("   %1").arg(formatValue(
                        dot.val_min, scl->axis_y_.fmt_value, scl->precision_ + 1));

                // Max Value
                if (dot.ct > 1 && (dot.val_min != dot.val_max))
                    curValue += QString("..%1").arg(formatValue(
                            dot.val_max, scl->axis_y_.fmt_value, scl->precision_ + 1));

                painter->drawText(QPointF(x, y), curValue);
                curValue = "";
                y += step;
            } while (false);
        }
    }

    if (stats_.count) {
        painter->drawText(QPointF(x, y), "N: " + QString::number(scl->stat_.ct));
        y += step;
    }
    if (stats_.minimum) {
        painter->drawText(
                QPointF(x, y),
                QString("min: %1").arg(formatValue(
                        scl->stat_.val_min, scl->axis_y_.fmt_value, scl->precision_ + 1)));
        y += step;
    }
    if (stats_.maximum) {
        painter->drawText(
                QPointF(x, y),
                QString("max: %1").arg(formatValue(
                        scl->stat_.val_max, scl->axis_y_.fmt_value, scl->precision_ + 1)));
        y += step;
    }
    if (stats_.m) {
        painter->drawText(QPointF(x, y),
                          QString("M: %1")
                                  .arg(static_cast<double>(scl->stat_.m))
                                  .replace(QLocale(QLocale::English).decimalPoint(),
                                           QLocale::system().decimalPoint()));
        y += step;
    }
    if (stats_.sd) {
        painter->drawText(QPointF(x, y),
                          QString("σ: %1")
                                  .arg(static_cast<double>(scl->stat_.sd))
                                  .replace(QLocale(QLocale::English).decimalPoint(),
                                           QLocale::system().decimalPoint()));
        y += step;
    }

    if (stats_.frequency) {
        if (scl->stat_.time_length > 0) {
            double freq = scl->stat_.ct / scl->stat_.time_length;
            painter->drawText(QPointF(x, y),
                              tr("F: %1 Гц")
                                      .arg(freq)
                                      .replace(QLocale(QLocale::English).decimalPoint(),
                                               QLocale::system().decimalPoint()));
            y += step;
        }
    }*/
}

//-------------------------------------------------------------------------
void QFormScreen::placeMarker(QPainter *painter, const QScreenMarker *mrk) {
    QPointF pt = mrk->scenePos();
    QRectF rt = mrk->img->rect();

    if (mrk->isVisible()) painter->drawPixmap(pt, QPixmap::fromImage(*mrk->img), rt);
    else
        return;

    // Markers
    if (!mrk->valid_) return;

    int x = static_cast<int>(pt.x() + MARKER_SHIFT);

    // Time Value
    QPointF ptT;
    ptT.setX(x);
    ptT.setY(scene_->height() - height_axis_x_ + 10);  // SCREEN_OFFSET_BOTTOM
    painter->setPen(mrk->color_);

    QFont ft = painter->font();
    ft.setPointSize(lining_.font_size);
    painter->setFont(ft);

    QString s = QString("%1")
                    .arg(mrk->t_.toDouble(), 0, 'f', 2, QChar('0'))
                    .replace(QLocale(QLocale::English).decimalPoint(), QLocale::system().decimalPoint());

    // TODO "axis_t_hms_" value is always true. is it normal?
    if (is_axis_t_hms_) s = secToHMS(mrk->t_);

    painter->drawText(ptT, s);

    placeMarkerValues(painter, x);
}

//-------------------------------------------------------------------------
void QFormScreen::placeMarkerFloat(QPainter *painter) {
    // if(!diagrams.count())
    //    return;

    if (!mark_f_.enabled) return;

    qreal sceneH = scene_->height();

    int x = static_cast<int>(mark_f_.x);

    mark_f_.t = axis_x_->scale_.at(x);

    QPen pen(Qt::black);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    QFont ft = painter->font();
    ft.setPointSize(lining_.font_size);
    painter->setFont(ft);

    painter->drawLine(QPointF(mark_f_.x, 0), QPointF(mark_f_.x, sceneH));
    if (axis_y_marker_)
        painter->drawLine(QPointF(margin_.left - kDiagramMargin, mark_f_.y),
                          QPointF(scene_->width() - margin_.right + kDiagramMargin, mark_f_.y));

    QPointF ptT;
    ptT.setX(x + 5);
    ptT.setY(scene_->height() - height_axis_x_ - 11);  // SCREEN_OFFSET_BOTTOM

    double pixelPow = log10(axis_x_->pixel_weight_);
    int prec = 0;
    if (pixelPow < 0) prec = static_cast<int>(fabs(pixelPow)) + 1;

    RelativeTime t = mark_f_.t + current_time_.end;
    // QString s = QString("%1").arg(t_, 0, 'f', prec);
    // if(dstx.axisT.hms)
    QString s = secToHMS(t, prec);

    QFontMetrics fm = painter->fontMetrics();
    int width = fm.width(s) + 2;
    int y = static_cast<int>(ptT.y() - lining_.font_size - 1);

    pen.setColor(Qt::white);
    painter->setPen(pen);
    painter->fillRect(static_cast<int>(ptT.x() - 2), y - 2, width + 1, lining_.font_size + 4, QBrush(Qt::black));
    painter->drawText(ptT, s);

    if (mark_a_.enabled) {
        RelativeTime dt = mark_f_.t - mark_a_.t;

        ptT.setY(scene_->height() - height_axis_x_ - 1);  // SCREEN_OFFSET_BOTTOM

        pen.setColor(Qt::black);
        painter->setPen(pen);
        painter->drawText(ptT, QString("Δt: %1").arg(dt.toDouble(), 0, 'f', prec));
    }

    if (axis_y_current_) placeMarkerValues(painter, x);
}

//-------------------------------------------------------------------------
void QFormScreen::placeMarkerAnchor(QPainter *painter) {
    // if(!diagrams.count())
    //    return;

    if (!mark_a_.enabled) return;

    qreal sceneH = scene_->height();
    int x = static_cast<int>(mark_a_.x);

    mark_a_.t = axis_x_->scale_.at(x);

    QPen pen(Qt::black);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    QFont ft = painter->font();
    ft.setPointSize(lining_.font_size);
    painter->setFont(ft);

    painter->drawLine(QPointF(mark_a_.x, 0), QPointF(mark_a_.x, sceneH));

    QPointF ptT;
    ptT.setX(x + 5);
    ptT.setY(scene_->height() - height_axis_x_ + 10);  // SCREEN_OFFSET_BOTTOM

    double pixelPow = log10(axis_x_->pixel_weight_);
    int prec = 0;
    if (pixelPow < 0) prec = static_cast<int>(fabs(pixelPow)) + 1;

    RelativeTime t = mark_a_.t + current_time_.end;
    QString s = secToHMS(t, prec);
    painter->drawText(ptT, s);

    if (axis_y_current_) placeMarkerValues(painter, x);
}

//-------------------------------------------------------------------------
void QFormScreen::placeMarkerValues(QPainter *painter, int x) {
    // QString s;
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
}

//-------------------------------------------------------------------------
QString QFormScreen::formatValue(const double &val, const DataOutputFormat fmt, const int &prec, const bool &trim) {
    QString s;

    switch (fmt.type) {
    case kDataOutputFormatInt16: {
        int ival = static_cast<int>(val);
        s = QString("%1").arg(ival, 0, 16);
    } break;
    case kDataOutputFormatInt10: {
        int ival = static_cast<int>(val);
        s = QString("%1").arg(ival);
    } break;
    case kDataOutputFormatInt8: {
        int ival = static_cast<int>(val);
        s = QString("%1").arg(ival, 0, 8);
    } break;
    case kDataOutputFormatInt2: {
        int ival = static_cast<int>(val);
        s = QString("%1").arg(ival, 0, 2);
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

//-------------------------------------------------------------------------
int QFormScreen::trimZeroes(const double &val0, const int &prec) {
    double val = val0;
    int ct = 0;

    while (val - floor(val) > 0) {
        val *= 10;
        ct++;

        if (ct > prec) break;
    }

    QString s = QString("%1").arg(val0, 0, 'f', ct);

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

//-------------------------------------------------------------------------
QString QFormScreen::secToHMS(const RelativeTime &val, const int &prec) {
    // TODO: Check RelativeTime conversion
    int x = val.ls_integer;
    int hh = static_cast<int>(floor(x / 3600));
    x -= (hh * 3600);
    int mm = static_cast<int>(floor(x / 60));
    x -= (mm * 60);

    int64_t fract = 100.0 * val.ms_fractional / pow(2, 32);

    QString s = QString("%1:%2:%3,%4")
                    .arg(hh, 2, 10, QChar('0'))
                    .arg(mm, 2, 10, QChar('0'))
                    .arg(x, 2, 10, QChar('0'))
                    .arg(fract, 2, 10, QChar('0'));

    return s;
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

    onRefresh(current_time_.end, true);
}
//-------------------------------------------------------------------------
void QFormScreen::onPause() {
    is_pause_ = !is_pause_;

    emit toPaused(is_pause_);
}
//-------------------------------------------------------------------------
void QFormScreen::onClearScene() {
    scene_->blockSignals(true);

    scale_index_ = -1;
    marker_index_ = -1;
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
void QFormScreen::onGroupupScene() {
    int x = kAxisYDiagInterval;
    int y = kScreenOffsetTop;

    int dx = kAxisYWidth;  // + AXIS_Y_DIAG_INTERVAL;
    int dy = kAxisYDiagInterval;

    int h = scene_->height() - kScreenOffsetTop;  // - SCREEN_OFFSET_BOTTOM;

    int c = 0;

    for (auto d : scales_) {
        d->setPos(x, y);

        y += (d->rect().height() + dy);

        if (y > (h - kAxisYDiagInterval)) {
            c++;
            x += dx;
            y = kScreenOffsetLeft + (0.5 * kAxisYHeightDefault * (c & 0x1));
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
    qreal sceneW = scene_->width();
    qreal sceneH = scene_->height();
    QPixmap pmScene(static_cast<int>(sceneW), static_cast<int>(sceneH));
    pmScene.fill(Qt::white);
    QPainter *painter = new QPainter(&pmScene);

    QFont ft = painter->font();
    ft.setPointSize(lining_.font_size);
    painter->setFont(ft);

    // Axis X (Time)
    placeAxisX(painter, axis_x_);

    // Diagrams & Scales
    do {
        const QScreenScale *last = nullptr;

        for (auto d : scales_) {
            if (d->getIndex() != scale_index_) placeDiag(painter, d);
            else
                last = d;

            warning |= d->warning_;
        }
        if (last != nullptr) placeDiag(painter, last);

        for (auto d : scales_) {
            if (d->getIndex() != scale_index_) {
                placeScale(painter, d);
                placeStat(painter, d);
            }
        }
        if (last != nullptr) {
            placeScale(painter, last);
            placeStat(painter, last);
        }
    } while (false);

    // Markers
    /*do {
        if (marker_index_ < 0)
            break;
        for (auto mrk: markers_) {
            if (mrk->index_ != marker_index_)
                placeMarker(painter, mrk);
        }
        placeMarker(painter, markers_.at(marker_index_));
    } while (false);*/

    // Simple Markers
    placeMarkerAnchor(painter);
    placeMarkerFloat(painter);

    delete painter;

    scene_->setBackgroundBrush(pmScene);

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
    QAction *actAddMarker = new QAction(tr("Добавить маркер"), this);
    actAddMarker->setData(pt);
    connect(actAddMarker, &QAction::triggered, this, &QFormScreen::onAddMarkerFromMenu);

    QAction *actAddLabel = new QAction(tr("Добавить надпись"), this);
    actAddLabel->setData(pt);
    connect(actAddLabel, &QAction::triggered, this, &QFormScreen::onAddLabelFromMenu);

    QMenu *menu = new QMenu(this);
    menu->addAction(actAddMarker);
    menu->addAction(actAddLabel);
    menu->exec(QCursor::pos());

    // menu->deleteLater();
}
//-------------------------------------------------------------------------
void QFormScreen::onAddMarker(const QPointF &pt) {
    QSizeF sz;
    sz.setWidth(scene_->width());
    sz.setHeight(scene_->height());

    int index = markers_.count();
    QScreenMarker *mrk = new QScreenMarker(index, pt, sz, lining_, this);

    mrk->pScale_ = &axis_x_->scale_;

    int x = static_cast<int>(pt.x());
    if (x < axis_x_->scale_.count()) mrk->setTime(axis_x_->scale_.at(x));

    connect(mrk, &QScreenMarker::to_removed, this, &QFormScreen::onRemoveItem);
    connect(mrk, &QScreenMarker::to_changed, this, &QFormScreen::onUpdateScene);
    connect(mrk, &QScreenMarker::to_focused, this, &QFormScreen::onUpdateFocus);
    connect(mrk, &QScreenMarker::to_focused, scene_, &QScreenScene::onSelectItem);

    connect(this, &QFormScreen::toSceneResized, mrk, &QScreenMarker::on_resize);
    connect(this, &QFormScreen::toIndexReduced, mrk, &QScreenMarker::on_indexReduce);

    // connect(axisX, &QScreenAxisX::to_changedInterval, mrk,
    // &QScreenMarker::on_setValid);

    markers_.push_back(mrk);
    scene_->addItem(mrk);

    emit mrk->to_focused(kSourceMarker, index);
    emit mrk->to_changed(kSourceMarker);
}
//-------------------------------------------------------------------------
void QFormScreen::onAddMarkerFromMenu() {
    QAction *actSrc = qobject_cast<QAction *>(sender());

    QPointF pt = qvariant_cast<QPointF>(actSrc->data());

    onAddMarker(pt);
}
//-------------------------------------------------------------------------
void QFormScreen::onAddLabel(const QPointF &pt) {}
//-------------------------------------------------------------------------
void QFormScreen::onAddLabelFromMenu() {
    QAction *actSrc = qobject_cast<QAction *>(sender());

    QPointF pt = qvariant_cast<QPointF>(actSrc->data());

    onAddLabel(pt);
}
//-------------------------------------------------------------------------
void QFormScreen::onAddMarkerAnchor(const QPointF &pt) {
    // if(!diagrams.count())
    //    return;

    if ((pt.x() < margin_.left) || (pt.x() > scene_->width() - margin_.right)) {
        mark_a_.enabled = false;
        return;
    }

    mark_a_.enabled = true;
    mark_a_.x = pt.x();
    mark_a_.y = pt.y();  // not used
    mark_a_.t = axis_x_->scale_.at(static_cast<int>(pt.x()));

    onUpdateScene(kSourceMarkerAnchor);
}

//-------------------------------------------------------------------------
void QFormScreen::onUpdateMarkerFloat(const QPointF &pt) {
    // if(!diagrams.count())
    //    return;

    if ((pt.x() < margin_.left) || (pt.x() > scene_->width() - margin_.right)) {
        mark_f_.enabled = false;
        return;
    }

    mark_f_.enabled = true;
    mark_f_.x = pt.x();
    mark_f_.y = pt.y();
    mark_f_.t = axis_x_->scale_.at(static_cast<int>(pt.x()));

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
    current_time_.bgn = current_time_.end - time_width_;

    axis_x_->setMargin(margin_);
    axis_x_->setInterval(current_time_, true);
    for (auto scale : scales_) {
        scale->setMargins(margin_);
        scale->setTiming(current_time_, {});
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

        scale_index_ = scales_.count() - 1;
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

        marker_index_ = markers_.count() - 1;
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
void QFormScreen::onTuneItem(const int &src, const int &index) {
    /*QFormDiagSettings   *fs = new QFormDiagSettings(this);

    connect(fs, &QFormDiagSettings::to_confirmed, this,
    &QFormDiag::on_changeSettings);

    int     indexTab    = 0;
    QString nameDiag    = "";
    int     indexLbl    = -1;

    switch(src){
    case SOURCE_DIAGRAM:
        nameDiag    = diagrams.at(index)->sName;
        indexTab    = 1;
        break;
    case SOURCE_LABEL:
        indexLbl    = index;
        indexTab    = 2;
        break;
    default:
        ;
    }

    fs->setStxCommon(sTitle, labelMain, dstx);
    fs->setStxSingle(nameDiag, getDiagNameList(), getDiagSettingsMap());
    fs->setStxLabel(indexLbl, getLabelCaptionList(), getLabelSettingsList());
    fs->setActiveTab(indexTab);
    fs->show();*/
}
//-------------------------------------------------------------------------
void QFormScreen::onAlignItem(const int &src, const int &index, const int &val) {
    if (src != kSourceScale) return;

    if (index < 0) return;

    QRectF rt;   // = scales.at(index)->rect();
    QPointF pt;  // = scales.at(index)->pos();

    qreal xMin, xMax, yMin, yMax;
    int ct = 0;
    for (auto d : scales_) {
        if (!d->isSelected()) continue;

        if (!ct) {
            xMin = d->pos().x();
            xMax = xMin;
            yMin = d->pos().y();
            yMax = yMin + d->rect().height();
        } else {
            xMin = std::min(xMin, d->pos().x());
            xMax = std::max(xMax, d->pos().x());
            yMin = std::min(yMin, d->pos().y());
            yMax = std::max(yMax, (d->pos().y() + d->rect().height()));
        }
        ct++;

        if (d->getIndex() == index) {
            rt = d->rect();
            pt = d->pos();
        }
    }

    for (auto d : scales_) {
        if (!d->isSelected()) continue;

        switch (val) {
        case 0:
            d->setPos(d->pos().x(), yMin);
            break;
        case 1:
            d->setPos(d->pos().x(), yMax - d->rect().height());
            break;
        case 2:
            d->setPos(xMin, d->pos().y());
            break;
        case 3:
            d->setPos(xMax, d->pos().y());
            break;
        default:
            if (d->getIndex() != index) {
                d->setRect(d->rect().x(), d->rect().y(), d->rect().width(), rt.height());
                d->drawDiag();
                d->drawScale();
            }
        }
    }

    onUpdateScene(kSourceMain);
}
//-------------------------------------------------------------------------
void QFormScreen::onSetAxisXHeight(const int &h) { height_axis_x_ = h; }
//-------------------------------------------------------------------------
void QFormScreen::onHideScale() {
    is_axis_hidden_ = !is_axis_hidden_;

    if (is_axis_hidden_) act_scale_hide_->setText(tr("Показать шкалы"));
    else
        act_scale_hide_->setText(tr("Скрыть шкалы"));

    act_scale_hide_->setChecked(is_axis_hidden_);
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

    /*QString     name = "";
    QStringList list;
    QMap<QString, SETTINGS_DIAGITEM>    m_dstx;

    for(QDiagItem *d : diagrams){
        list.push_back(d->sName);
        m_dstx.insert(d->sName, d->dstx);
    }
    if(!list.isEmpty())
        name = list.first();

    QFormDiagSettings   *fs = new QFormDiagSettings(this);

    connect(fs, &QFormDiagSettings::to_confirmed, this, &QFormDiag::on_changeSettings);

    fs->setStxCommon(sTitle, labelMain, dstx);
    fs->setStxSingle("", list, m_dstx);
    fs->setActiveTab(0);
    fs->show();*/
}
//-------------------------------------------------------------------------
void QFormScreen::onSaveConf() {
    /*QSettings   ini(g_supportPath + INI_FILE_NAME_EXP, QSettings::IniFormat);
    QString     path = ini.value(INI_RAW_SAVE, g_supportPath).toString();

    QFileDialog dlg(this, tr("Сохранение конфигурации экрана"));
    dlg.setDefaultSuffix(CONF_FILE_EXT);

    dlg.setNameFilters(QStringList() << tr("Файл конфигурации экрана
    (*%1)").arg(CONF_FILE_EXT));
    dlg.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);

    int result = dlg.exec();
    if(result != QFileDialog::Accepted)
        return;

    QString s = QFileInfo(dlg.selectedFiles().first()).fileName();
    s = s.left(s.lastIndexOf(CONF_FILE_EXT));
    if(sTitle.isEmpty())
        sTitle = s;

    QFileDiagConfiguration  *f = new QFileDiagConfiguration(this);

    f->add(0, sTitle, labelMain, geometry(), dstx, axisX, diagrams, markers,
    labels); bool    success = f->save(dlg.selectedFiles().first()); if(!success)
        return;*/
}
//-------------------------------------------------------------------------
void QFormScreen::onProgress(const int &index) {
    if (index < 0) return;

    counter_++;

    int value = scales_.isEmpty() ? 100 : (100 * counter_ / scales_.count());

    if (index == scales_.count() - 1) value = 100;

    if (counter_ == scales_.count()) counter_ = 0;

    progress_->setValue(value);
    progress_->setVisible(static_cast<bool>(100 - value));
}

//-------------------------------------------------------------------------
void QFormScreen::onProgress2(const int &j, const int &n) {
    if (n <= 0) progress_->setValue(0);
    else
        progress_->setValue(100 * j / n);

    progress_->setVisible(static_cast<bool>(n - j));
}
//-------------------------------------------------------------------------
void QFormScreen::onReport(const QString &s) { statusbar_->showMessage(s); }
//-------------------------------------------------------------------------
void QFormScreen::onMakeShotManual() {
    QSettings ini(g_support_path + ini_filename_eo_, QSettings::IniFormat);
    QString path = ini.value(ini_image_save_path_, g_support_path).toString();

    QString selected_filter;

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
