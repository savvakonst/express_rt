#ifndef QSCREENSCENE_H
#define QSCREENSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>

#include "qscreenscale.h"
//#include "Helper_Works2.h"

class Parameter_ifs;
class Device_ifs;
class ExtensionManager;

class QScreenScene : public QGraphicsScene {
    Q_OBJECT
   public:
    explicit QScreenScene(ExtensionManager* manager, QWidget* parent = nullptr);
    ~QScreenScene() override;

    QRect setRect(const QRect& rt_0);

    ExtensionManager* manager_ = nullptr;
   public slots:
    void onSelectItem(const int& index);

   private:
    bool b_left_pressed_ = false;
    bool b_right_pressed_ = false;

    QPointF pt_left_;
    QPointF pt_right_;

   protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    /// void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    // void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    // void dropEvent(QGraphicsSceneDragDropEvent* event) override;

   signals:

    void toMenuCalled(const QPointF& pt);
    void toMarkerPlaced(const QPointF& pt);
    void toLeftGestured(const QPointF& pt_1, const QPointF& pt_2);
    void toLeftClicked(const QPointF& pt);
    void toRightGestured(const QPointF& pt_1, const QPointF& pt_2);
    void toRightClicked(const QPointF& pt);
    void toMouseWheeled(const QPointF& pt, const int& delta);
    void toPaused();
    void toMouseMoved(const QPointF& pt);
    void toDropParameter(const QPointF& pt, const std::string& name);
};

#endif  // QSCREENSCENE_H
