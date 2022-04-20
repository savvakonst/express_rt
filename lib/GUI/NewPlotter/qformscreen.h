#ifndef EXRT_QFORMSCREEN_H
#define EXRT_QFORMSCREEN_H

#include <QAction>
#include <QCloseEvent>
#include <QDateTime>
#include <QDialog>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QResizeEvent>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>

#include "qformscreensettings.h"
#include "qscreenaxisx.h"
#include "qscreenmarker.h"
#include "qscreenscale.h"
#include "qscreenscene.h"

namespace Ui {
class QFormScreen;
}

class ExtensionManager;

/**
 * \brief  Главное окно экрана содержащее в себе сцену, меню настроек и прочее
 */

class QFormScreen : public QDialog {
    Q_OBJECT

    const QString ini_filename_eo_ = "/ksdeo.ini";
    const QString ini_image_save_path_ = "IMAGE_SAVE/PATH";
    const QString ini_image_save_auto_ = "IMAGE_SAVE/AUTO";

   public:
    explicit QFormScreen(ExtensionManager *manager, PlotterContext_ifs *plotter_context, QWidget *parent = nullptr);

    ~QFormScreen() override;

    QSizeF getSceneSize();



    void setInterval(const TimeInterval &ti_0);

    QScreenScale *addScale(QScreenScale *p_scale);

    QScreenScale *addScale(Reader_ifs *reader);

    QScreenScale *getScale(const int &index);

    int scale_index_ = -1;
    int marker_index_ = -1;
    int label_index_ = -1;

   public slots:

    void onExit();

    // void onIndexReduce(const int &index_0);

    void onRefresh(const RelativeTime &t, bool zoomed = false);

    void onSetTimeVariables(const RelativeTime &t_width, const RelativeTime &t_step);

    void onAddLabelExt(const RelativeTime &t_0, const int &y_0, const QString &txt, const LineProperties &properties);

    void onAddMakerExt(const RelativeTime &t_0);

    void onMessageShow(const QString &s);

   private:
    struct AxisYStats {
        union {
            struct {
                bool enabled;
                bool current;
                bool count;
                bool maximum;
                bool minimum;
                bool m;
                bool sd;
                bool frequency;
            };
            bool b[8];
        };
    };

    bool saveShot(const QString &filename);

    static void placeAxisX(QPainter *painter, const QScreenAxisX *axis);

    void placeMarker(QPainter *painter, const QScreenMarker *mrk);

    /**
     * Отрисовывает маркер и дистанцию distance если distance != RelativeTime{0,0}
     */

    void placeTimeIndependentMarker(QPainter *painter, MarkerSimple &marker, RelativeTime distance);

    void placeMarkerValues(QPainter *painter, int x);

    PlotterContext_ifs *plotter_context_;
    ExtensionManager *manager_;

    Ui::QFormScreen *ui_;
    QMenuBar *menubar_;
    QToolBar *toolbar_;
    QStatusBar *statusbar_;
    QProgressBar *progress_;

    QMenu *mn_file_;
    QMenu *mn_settings_;

    QAction *act_exit_;
    QAction *act_shot_;
    QAction *act_shot_as_;
    QAction *act_clear_;
    QAction *act_refresh_;
    QAction *act_group_up_;
    QAction *act_scale_hide_;

    QAction *act_settings_;
    QAction *act_conf_save_;

    QScreenScene *scene_;
    QScreenAxisX *axis_x_;

    QList<QScreenScale *> scales_;
    QList<QScreenMarker *> markers_;
    // QList<QScreenLabel*>                labels;

    QString title_ = "";

    // RelativeTime t_ = {0};

    // Float Marker
    MarkerSimple mark_f_;
    MarkerSimple mark_a_;


    // Settings
    bool is_axis_t_hms_ = true;
    bool is_axis_hidden_ = false;
    int height_axis_x_ = int(kScreenOffsetBottom);

    AxisYStats stats_{};

    // TODO: only one occurrence in the code. no assignments. maybe remove it?
    bool axis_y_marker_ = false;
    bool axis_y_current_ = true;
    
    RelativeTime time_width_ = {0};
    Timing time_;

    ImageSaving saving_;
    LineProperties lining_;
    Margin margin_;

    QString program_data_path_ = "";

    bool list_diag_enabled_ = false;
    bool list_marker_enabled_ = false;
    bool list_label_enabled_ = false;

    int counter_ = 0;

   private slots:

    void onResizeScene();
    void onZoom(const QPointF &pt, const int &delta);


    void onClearScene();

    void onRefreshScene();

    void onGroupScene();

    void onUpdateScene(const int &src = kSourceMain);

    void onUpdateFocus(const int &src, const int &index_last);

    void onPopupMenuAdd(const QPointF &pt);

    void onAddMarker(const QPointF &pt);

    void onAddMarkerFromMenu();

    void onAddLabel(const QPointF &pt);

    void onAddLabelFromMenu();

    void onAddMarkerAnchor(const QPointF &pt);

    void onUpdateMarkerFloat(const QPointF &pt);

    void onChangeSettings(const QString &title, const SettingsCommon &stx_cmn);

    // void                                on_addParameter();

    void onRemoveItem(const int &src = -1, const int &index = -1);
    void onTuneItem(const int &src = -1, const int &index = -1);
    void onAlignItem(const int &src = -1, const int &index = -1, const int &val = 0);

    void onSetAxisXHeight(const int &h);

    void onHideScale();

    void onShowSettings();

    void onSaveConf();

    void onReport(const QString &s);

    void onMakeShotManual();

    void onMakeShotAuto();

    void onSaveImageFilterChange(const QString &filter);

   protected:
    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

   signals:


    void toSceneChanged();

    void toSceneResized(const qreal &w, const qreal &h);

    void toMouseMoved(QPointF);

    void toIndexReduced(const int &src, const int &index);

    void toTitleChanged();

    void toHelp();
};

#endif
