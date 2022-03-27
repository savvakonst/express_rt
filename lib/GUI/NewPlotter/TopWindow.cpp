//
// Created by SVK on 27.03.2022.
//

#include "TopWindow.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

TopWindow::TopWindow(QWidget *parent) {
    setAcceptDrops(true);
    setWindowTitle(tr("MDI"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void TopWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasFormat("text/module")) {
        e->acceptProposedAction();
    }
}

/*

void TopWindow::dragMoveEvent(QDragMoveEvent *event) {
    qDebug() << "dropEvent";
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}

void TopWindow::dragLeaveEvent(QDragLeaveEvent *event) {
    qDebug() << "dropEvent";
    event->accept();
}
*/

void TopWindow::dropEvent(QDropEvent *e) {
    if (e->mimeData()->hasFormat("text/module")) {
        qDebug() << "Dropped file:" << e->mimeData()->data("text/module").data();
    }
}