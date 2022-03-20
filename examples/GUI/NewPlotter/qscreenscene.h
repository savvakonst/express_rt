#ifndef QSCREENSCENE_H
#define QSCREENSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>

//#include "Helper_Works2.h"

class QScreenScene : public QGraphicsScene {
    Q_OBJECT
   public:
    explicit QScreenScene(QWidget* parent = nullptr);
    ~QScreenScene() override;

    QRect setRect(const QRect& rt_0);

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

    // void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    // void dropEvent(QGraphicsSceneDragDropEvent *event) override;

   signals:
    void to_menuCalled(const QPointF pt);
    void to_markerPlaced(const QPointF pt);
    void to_leftGestured(const QPointF& pt1, const QPointF& pt2);
    void to_leftClicked(const QPointF& pt);
    void to_rightGestured(const QPointF& pt1, const QPointF& pt2);
    void to_rightClicked(const QPointF& pt);
    void to_mouseWheeled(const QPointF& pt, const int& delta);
    void to_paused();

    void to_mouseMoved(const QPointF& pt);
};

#endif  // QSCREENSCENE_H
