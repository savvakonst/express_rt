

#ifndef EXRT_MAINWINDOW_H
#define EXRT_MAINWINDOW_H

#include <QGraphicsScene>
#include <QMainWindow>
#include <QWidget>
#include <atomic>

#include "common/Reader_ifs.h"
#include "convtemplate/PrmBuffer_ifs.h"

QT_BEGIN_NAMESPACE

class QGraphicsView;

QT_END_NAMESPACE

class TopWindow : public QMainWindow {
    Q_OBJECT
   public:
    TopWindow(QWidget* parent = 0, double signal_frequency = 0.1);
    ~TopWindow();
    void redraw();

   private:
    // void setupMatrix();
    void initScene();

    void draw(QPainter& paint, Reader_ifs::Chunk* c_ptr, size_t offset) const;

    size_t hight_ = 800;
    size_t buffer_size_ = 2000;
    size_t half_h_ = hight_ / 2;
    size_t scale_h_ = half_h_ / 4;

    Reader_ifs::Point* buffer_1_ = nullptr;
    Reader_ifs::Point* buffer_2_ = nullptr;
    PrmBuffer_ifs* reader_ = nullptr;
    QGraphicsScene* scene_ = nullptr;
    QGraphicsView* view_ = nullptr;

    const double signal_frequency_;

    size_t time_ = 0;
    size_t sample_frequency_log2_ = 0;

   public:
    RelativeTime time_offset_ = {0, 0};
    RelativeTime time_interval_ = {0, 30};

    Reader_ifs* view_1_ = nullptr;
    Reader_ifs* view_2_ = nullptr;

    ExtensionManager* manager_ = nullptr;

    friend class GraphicsScene;
};

class GraphicsScene : public QGraphicsScene {
   public:
    GraphicsScene(QObject* parent = nullptr) : QGraphicsScene(parent) {}

   protected:
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
};

#endif  // EXRT_MAINWINDOW_H