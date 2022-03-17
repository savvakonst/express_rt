#include "qscreenscene.h"
#include "qscreenaxisx.h"
//-------------------------------------------------------------------------
QScreenScene::QScreenScene(QWidget* parent): QGraphicsScene(nullptr){
    Q_UNUSED(parent);

}
//-------------------------------------------------------------------------
QScreenScene::~QScreenScene(){

}
//-------------------------------------------------------------------------
QRect QScreenScene::setRect(const QRect& rt0){
    QRect   rt = rt0;
    rt.setTop(rt0.top());
    rt.setLeft(rt0.left());
    rt.setWidth(rt0.width() - 4);
    rt.setHeight(rt0.height() - 4);

    setSceneRect(rt);

    return rt;
}
//-------------------------------------------------------------------------
void QScreenScene::on_selectItem(const int& index){
    Q_UNUSED(index);

    bLeftPressed    = false;
    bRightPressed   = false;
}
//-------------------------------------------------------------------------
void QScreenScene::keyPressEvent(QKeyEvent* event){
    switch(event->key()){
    case Qt::LeftArrow:
        break;
    case Qt::RightArrow:
        break;
    case Qt::Key_Pause:
        emit to_paused();
        break;
    default:
        ;
    }

    update();
    QGraphicsScene::keyPressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::keyReleaseEvent(QKeyEvent* event){
    update();
    QGraphicsScene::keyReleaseEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::mousePressEvent(QGraphicsSceneMouseEvent* event){
    switch(event->button()){
    case Qt::LeftButton:
        ptLeft = event->scenePos();
        bLeftPressed = true;
        break;
    case Qt::RightButton:
        ptRight = event->scenePos();
        bRightPressed = true;
        break;
    default:
        ;
    }

    do{
        QPointF pt = event->scenePos();

        if(pt.x() > (width() - DIAGRAM_OFFSET_RIGHT))
            break;
        if(pt.x() < DIAGRAM_OFFSET_LEFT)
            break;

        emit to_leftClicked(pt);
    } while(false);

    update();
    QGraphicsScene::mousePressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event){
    QPointF     pt = event->scenePos();

    int         leftX   = DIAGRAM_OFFSET_LEFT;
    int         rightX  = DIAGRAM_OFFSET_RIGHT;

    switch(event->button()){
    case Qt::LeftButton:
    {
        do{
            if(event->modifiers() & Qt::SHIFT){
                emit to_markerPlaced(ptLeft);
                break;
            }
            if(bLeftPressed){
                bLeftPressed = false;

                if((pt.x() > ptLeft.x()) && (pt.x() < leftX))
                    break;

                if((pt.x() > ptLeft.x()) && (ptLeft.x() > rightX))
                    break;

                if(abs((pt.x() - ptLeft.x())) <= 1)
                    break;

                emit to_leftGestured(ptLeft, pt);
            }
        } while(false);
    }
    break;
    case Qt::RightButton:
    {
        do {
            if(event->modifiers() & Qt::CTRL){
                emit to_menuCalled(pt);
                break;
            }
            if(event->modifiers() & Qt::SHIFT){

            }
            if(bRightPressed){
                bRightPressed = false;

                if((pt.x() >= ptRight.x()) && (pt.x() < leftX))
                    break;

                if((pt.x() > rightX) && (ptRight.x() > rightX))
                    break;

                if(pt.x() > rightX)
                    pt.setX(rightX);

                if(pt.x() < leftX)
                    pt.setX(leftX);

                emit to_rightGestured(ptRight, pt);
            }
        } while(false);
    }
    break;
    default:
        ;
    }

    update();
    QGraphicsScene::mouseReleaseEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event){
    emit to_mouseMoved(event->scenePos());

    update();
    QGraphicsScene::mouseMoveEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event){
    switch(event->button()){
    case Qt::LeftButton:
        break;
    default:
        ;
    }

    update();
    QGraphicsScene::mouseDoubleClickEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::wheelEvent(QGraphicsSceneWheelEvent* event){
    QPointF pt = event->scenePos();

    do {
        if(pt.x() < DIAGRAM_OFFSET_LEFT)
            break;
        if(pt.x() > (width() - DIAGRAM_OFFSET_RIGHT))
            break;

        if(event->delta() == 0)
            break;

        emit to_mouseWheeled(pt, event->delta());
    } while(false);

    update();
    QGraphicsScene::wheelEvent(event);
}
//-------------------------------------------------------------------------
