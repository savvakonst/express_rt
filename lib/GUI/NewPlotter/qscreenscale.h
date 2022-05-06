#ifndef QSCREENSCALE_H
#define QSCREENSCALE_H

#include <QAction>
#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QImage>
#include <QKeyEvent>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QRect>
#include <QSizeF>
#include <memory>

#include "ReaderExample/ReaderExample.h"
#include "common/Reader_ifs.h"
#include "qformlevel.h"
#include "qscreenaxisx.h"

struct AxisYCutoff {
    double val;
    int pos;
    bool enabled;
    bool reserved[3];
};

struct AxisXyDot {
    double val_max = 0;
    double val_min = 0;

    int y0 = 0;
    int y1 = 0;

    int x = 0;
    int ct = 0;
    bool ghost = true;
    bool warning = false;
};

struct AxisYStatistics {
    qint64 ct = 0;
    double val_max = 0;
    double val_min = 0;
    long double m = 0;
    long double d = 0;
    // long double         mo;
    long double sd = 0;
    //
    double time_length = 0;
};

struct PrmAttributes {
    // AxisYStatistics stat;
    Reader_ifs *reader;
    Reader_ifs::Point *buffer;
    std::unique_ptr<Reader_ifs::Chunk> chunks;
    std::vector<ControlLevel> *plevels;
    QString label_;
};

class Parameter_ifs;
class Device_ifs;

class QScreenScale : public QObject, public QGraphicsRectItem {
    Q_OBJECT

   public:
    struct AxisYSettings {
        DataOutputFormat fmt_value;
        DataOutputFormat fmt_scale;
        bool greed = false;
    };

    struct AxisSettings {
        bool automatic = true;
        qreal value = 10;
    };

    static const QList<uint32_t> diag_colors_;

    /*, 0x8D6E63 , 0x795548 , 0x6D4C41 , 0x5D4037 , 0x4E342E , 0x3E2723;*/

    explicit QScreenScale(Reader_ifs *reader, const int &index, const QSizeF &sz, const LineProperties &dstx,
                          const Margin &margin, QWidget *parent);

    explicit QScreenScale(const int &index, const QSizeF &sz, const LineProperties &dstx, const Margin &margin,
                          QWidget *parent);

    ~QScreenScale() override;

    bool addReader(Reader_ifs *reader);

    [[nodiscard]] int getIndex() const;

    /**
     * передает в painter, сформированное методом QScreenScale::drawPoints(), изображение точек графика для отрисовки
     */

    void placePoints(QPainter *painter) const;

    /**
     * передает в painter сформированное методом QScreenScale::formScaleImage() изображение шкал
     */

    void placeScale(QPainter *painter, bool is_axis_hidden);

    void placeStat(QPainter *painter);

    void setTemporaryValues(double val_min = -100, double val_max = 100);

    void setScale(QVector<RelativeTime> *xs);

    void setTiming(const TimeInterval &ti_0, const RelativeTime &step);

    void setMargins(const Margin &margin);

    void advance(int phase) override;

    [[nodiscard]] int type() const override;

    void recountScaleValues(const int &w, AxisYStatistics &stat, Reader_ifs::Chunk *c_ptr);

    /**
     *  формирует и сохраняет до следующего вызова изображение (QImage) точек графика
     */

    void formPointsImage();

    /**
     *  формирует и сохраняет до следующего вызова изображение (QImage) шкалы графика
     */

    void formScaleImage();

    void createPopupMenu(const QPoint &pt);

    QString s_label_ = "no_name";

    // Settings
    QRgb color_ = Qt::gray;

    AxisYSettings axis_y_;

    AxisSettings maximum_ = AxisSettings();
    AxisSettings minimum_ = AxisSettings();

    AxisSettings step_ = AxisSettings();

    LineProperties lining_;
    Margin margin_;

    TimeInterval ti_;

    QVector<RelativeTime> *p_scale_{};

    std::vector<AxisYCutoff> cut_off_;
    std::vector<ControlLevel> levels_;

    int img_shift_y_ = 0;
    QImage *img_points_;
    QImage *img_scale_;

    int precision_ = -1;
    int precision_scale_ = -1;

    QList<QVector<AxisXyDot>> list_dots_;

    AxisYStatistics stat_;
    bool interval_on_ = false;

    /*
     * this field signals that picture should be saved
     */

    bool warning_ = false;

   public slots:

    void onRemove();

    void onResize(const qreal &w, const qreal &h);

    void onSetX(const int &index, const int &x);

    void onIndexReduce(const int &src, const int &index_0);

    void onSettingsShow();

    void onAddLevelBegin() const;

    void onAddLevelEnd(const ControlLevel &lvl, const bool &flag, const int &index);

   private:
    void changeScaleBorder(const bool &high, const int &delta);

    QAction *act_settings_;
    QAction *act_remove_;

    QAction *act_level_add_;

    QMenu *menu_align_;
    QAction *act_align_t_;
    QAction *act_align_b_;
    QAction *act_align_l_;
    QAction *act_align_r_;
    QAction *act_align_h_;

    QMenu *menu_scale_;
    QAction *act_scale_auto_;
    QAction *act_scale_fix_;

    const int type_ = kSourceScale;

    std::list<PrmAttributes> prm_attributes_list_;

    bool disabled_ = false;
    QPair<qint64, qint64> offs_;
    int X_ = -1;

    int index_ = 0;
    int delta_x_ = 0;

    QSizeF scene_size_;

    bool left_pressed_ = false;
    bool right_pressed_ = false;

   private slots:

    void onAlignT();
    void onAlignB();
    void onAlignL();
    void onAlignR();
    void onAlignH();

    void onScaleAuto();
    void onScaleFix();

   protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void wheelEvent(QGraphicsSceneWheelEvent *event) override;

   signals:

    void toTuned(const int &src, const int &index);

    void toRemoved(const int &src, const int &index);

    void toChanged(int src);

    void toFocused(const int &src, const int &index);

    void toAlign(const int &src, const int &index, const int &val);
};

#endif  // QSCREENSCALE_H
