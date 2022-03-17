#ifndef QFORMSCREEN_H
#define QFORMSCREEN_H

#include <QDialog>
#include <QStatusBar>
#include <QProgressBar>
#include <QMenuBar>
#include <QToolBar>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QMessageBox>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QFileDialog>
#include <QDateTime>
#include <QStandardPaths>
#include <QTimer>

#include "qscreenscene.h"
#include "qscreenaxisx.h"
#include "qscreenmarker.h"
#include "qscreenscale.h"

#include "qformscreensettings.h"


namespace Ui {
    class QFormScreen;
}

class QFormScreen : public QDialog
{
    Q_OBJECT

    const QString INI_FILENAME_EO = "/ksdeo.ini";
    const QString INI_IMAGE_SAVE_PATH = "IMAGE_SAVE/PATH";
    const QString INI_IMAGE_SAVE_AUTO = "IMAGE_SAVE/AUTO";

public:


    explicit QFormScreen(const int &index, const LineProperties &dstx0, QWidget *parent = nullptr);

    explicit QFormScreen(/*const int &index, const SETTINGS_DIAGITEM &dstx0,*/ QWidget *parent = nullptr);

    ~QFormScreen();

    int getIndex();

    void setTitle(const QString &title);

    QString getTitle();

    QSizeF getSceneSize();

    void setSettings(const LineProperties &dstx);

    void setInterval(const TimeInterval &ti0);

    QScreenScale *addScale(Reader_ifs *reader);
    QScreenScale *addScale(/*const QString &name, QFileParameter *prm*/);

    QScreenScale *getScale(const int &index);

    int scale_index_ = -1;
    int marker_index_ = -1;
    int label_index_ = -1;

public slots:

    void onExit();

    void onIndexReduce(const int &index0);

    void onRefresh(const RelativeTime &t, const bool zoomed = false);

    void onTimer();

    void onSetTimeVariables(const RelativeTime &t_width, const RelativeTime &t_step);

    void onAddLabelExt(const RelativeTime &t0, const int &y0, const QString &txt, const LineProperties &dstx);

    void onAddMakerExt(const RelativeTime &t0);

    void onMessageShow(QString s);

    void on_help();

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

    void placeAxisX(QPainter *painter, const QScreenAxisX *axis);

    void placeDiag(QPainter *painter, const QScreenScale *scl);

    void placeScale(QPainter *painter, const QScreenScale *scl);

    void placeStat(QPainter *painter, const QScreenScale *scl);

    void placeMarker(QPainter *painter, const QScreenMarker *mrk);

    void placeMarkerFloat(QPainter *painter);

    void placeMarkerAnchor(QPainter *painter);

    void placeMarkerValues(QPainter *painter, int x);

    QString formatValue(const double &val, const DataOutputFormat fmt, const int &prec = -1, const bool &trim = false);

    int trimZeroes(const double &val, const int &prec = -1);

    QString secToHMS(const RelativeTime &val, const int &prec = QLocale::FloatingPointShortest);

    Ui::QFormScreen *ui_;
    QMenuBar *menubar_;
    QToolBar *toolbar_;
    QStatusBar *statusbar_;
    QProgressBar *progress_;

    QMenu *mn_file_;
    QAction *act_exit_;
    //QAction                             *actAdd;
    QAction *act_shot_;
    QAction *act_shot_as_;
    QAction *act_clear_;
    QAction *act_refresh_;
    QAction *act_group_up_;
    QAction *act_scale_hide_;
    //QAction                             *actListScale;
    //QAction                             *actListMarker;
    //QAction                             *actListLabel;

    QMenu *mn_settings_;
    QAction *act_settings_;
    //QMenu                               *mnConf;
    QAction *act_conf_save_;

    QMenu *mn_help_;
    QAction *act_help_;

    QAction *act_next_;
    QAction *act_prev_;
    QToolButton *toolbtn_goto_;

    QScreenScene *scene_;
    QScreenAxisX *axis_x_;

    QList<QScreenScale *> scales_;
    QList<QScreenMarker *> markers_;
    //QList<QScreenLabel*>                labels;

    int index_ = 0;
    QString title_ = "";

    TimeInterval ti_;
    RelativeTime t_last_ = {0};

    RelativeTime t_ = {0};

    // Example
    QTimer *timer_;
    //

    // Float Marker
    MarkerSimple mark_f_;
    MarkerSimple mark_a_;

    bool is_pause_ = false;
    // Settings
    bool is_axis_t_hms_ = true;
    bool is_axis_hidden_ = false;
    int height_axis_x_ = SCREEN_OFFSET_BOTTOM;

    AxisYStats stats_;

    // TODO: only one occurrence in the code. no assignments. maybe remove it?
    bool axis_y_marker_ = false;
    bool axis_y_current_ = true;


    QString label_main_ = "";
    //RelativeTime t_width_ = {0};
    //RelativeTime t_step_ = {0};

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
    void onZoom(const QPointF& pt, const int& delta);
    void onPause();

    void onClearScene();

    void onRefreshScene();

    void onGroupupScene();

    void onUpdateScene(const int &src = SOURCE_MAIN);

    void onUpdateFocus(const int &src, const int &index_last);

    void onPopupMenuAdd(const QPointF &pt);

    void onAddMarker(const QPointF &pt);

    void onAddMarkerFromMenu();

    void onAddLabel(const QPointF &pt);

    void onAddLabelFromMenu();

    void onAddMarkerAnchor(const QPointF &pt);

    void onUpdateMarkerFloat(const QPointF &pt);

    void onChangeSettings(const QString title, const SettingsCommon stx_cmn);

    //void                                on_addParameter();

    void onRemoveItem(const int &src = -1, const int &index = -1);
    void onTuneItem(const int &src = -1, const int &index = -1);
    void onAlignItem(const int &src = -1, const int &index = -1, const int &val = 0);

    void onSetAxisXHeight(const int &h);

    void onHideScale();

    void onShowSettings();

    void onSaveConf();

    void onProgress(const int &index);

    void onProgress2(const int &j, const int &n);

    void onReport(const QString &s);

    void onMakeShotManual();

    void onMakeShotAuto();

    void onSaveImageFilterChange(const QString &filter);

protected:
    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

signals:

    void toRemoved(const int &index);

    void toPaused(const bool is_paused);

    void toSceneChanged();

    void toSceneResized(const qreal &w, const qreal &h);

    void toMouseMoved(QPointF);

    void toIndexReduced(const int &src, const int &index);

    //void                                to_setInterval(const TIME_INTERVAL &ti);

    void to_titleChanged();

    void to_help();


};

#endif // QFORMSCREEN_H
