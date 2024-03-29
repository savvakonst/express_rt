﻿#include "qscreenscene.h"

#include <QMimeData>

#include "qscreenaxisx.h"
//
#include "GUI/WidgetWrappers.h"
#include "common/ExtensionManager.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/Parameter_ifs.h"

//-------------------------------------------------------------------------
QScreenScene::QScreenScene(ExtensionManager* manager, QWidget* parent) : manager_(manager), QGraphicsScene(nullptr) {
    Q_UNUSED(parent);

    // setAcceptDrops(true);
}
//-------------------------------------------------------------------------
QScreenScene::~QScreenScene() = default;
//-------------------------------------------------------------------------
QRect QScreenScene::setRect(const QRect& rt_0) {
    QRect rt = rt_0;
    rt.setTop(rt_0.top());
    rt.setLeft(rt_0.left());
    rt.setWidth(rt_0.width() - 4);
    rt.setHeight(rt_0.height() - 4);

    setSceneRect(rt);

    return rt;
}
//-------------------------------------------------------------------------
void QScreenScene::onSelectItem(const int& index) {
    Q_UNUSED(index);

    b_left_pressed_ = false;
    b_right_pressed_ = false;
}
//-------------------------------------------------------------------------
void QScreenScene::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::LeftArrow:
        break;
    case Qt::RightArrow:
        break;
    case Qt::Key_Pause:
        // TODO: some code which will produce pause 
        break;
    default:;
    }

    update();
    QGraphicsScene::keyPressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::keyReleaseEvent(QKeyEvent* event) {
    update();
    QGraphicsScene::keyReleaseEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    switch (event->button()) {
    case Qt::LeftButton:
        pt_left_ = event->scenePos();
        b_left_pressed_ = true;
        break;
    case Qt::RightButton:
        pt_right_ = event->scenePos();
        b_right_pressed_ = true;
        break;
    default:;
    }

    do {
        QPointF pt = event->scenePos();

        if (pt.x() > (width() - kDiagramOffsetRight)) break;
        if (pt.x() < kDiagramOffsetLeft) break;

        emit toLeftClicked(pt);
    } while (false);

    update();
    QGraphicsScene::mousePressEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    QPointF pt = event->scenePos();

    int leftX = kDiagramOffsetLeft;
    int rightX = kDiagramOffsetRight;

    switch (event->button()) {
    case Qt::LeftButton: {
        do {
            if (event->modifiers() & Qt::SHIFT) {
                emit toMarkerPlaced(pt_left_);
                break;
            }
            if (b_left_pressed_) {
                b_left_pressed_ = false;

                if ((pt.x() > pt_left_.x()) && (pt.x() < leftX)) break;

                if ((pt.x() > pt_left_.x()) && (pt_left_.x() > rightX)) break;

                if (abs((pt.x() - pt_left_.x())) <= 1) break;

                emit toLeftGestured(pt_left_, pt);
            }
        } while (false);
    } break;
    case Qt::RightButton: {
        do {
            if (event->modifiers() & Qt::CTRL) {
                emit toMenuCalled(pt);
                break;
            }
            if (event->modifiers() & Qt::SHIFT) {
            }
            if (b_right_pressed_) {
                b_right_pressed_ = false;

                if ((pt.x() >= pt_right_.x()) && (pt.x() < leftX)) break;

                if ((pt.x() > rightX) && (pt_right_.x() > rightX)) break;

                if (pt.x() > rightX) pt.setX(rightX);

                if (pt.x() < leftX) pt.setX(leftX);

                emit toRightGestured(pt_right_, pt);
            }
        } while (false);
    } break;
    default:;
    }

    update();
    QGraphicsScene::mouseReleaseEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    emit toMouseMoved(event->scenePos());

    update();
    QGraphicsScene::mouseMoveEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    switch (event->button()) {
    case Qt::LeftButton:
        break;
    default:;
    }

    update();
    QGraphicsScene::mouseDoubleClickEvent(event);
}
//-------------------------------------------------------------------------
void QScreenScene::wheelEvent(QGraphicsSceneWheelEvent* event) {
    QPointF pt = event->scenePos();

    do {
        if (pt.x() < kDiagramOffsetLeft) break;
        if (pt.x() > (width() - kDiagramOffsetRight)) break;

        if (event->delta() == 0) break;

        emit toMouseWheeled(pt, event->delta());
    } while (false);

    update();
    QGraphicsScene::wheelEvent(event);
}
