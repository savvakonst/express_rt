//
// Created by SVK on 27.03.2022.
//

#include <QCoreApplication>
#include <QDebug>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

///
#include "TopWindow.h"
#include "common/StringProcessingTools.h"
#include "device/DeviceManager.h"
#include "qformscreen.h"

TopWindow::TopWindow(QWidget *parent) {
    setAcceptDrops(true);
    setWindowTitle(tr("MDI"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void TopWindow::init(ExtensionManager *extension_manager) {
    device_manager_ =
        (DeviceManager *)extension_manager->getLastVersionExtensionObject("device_manager", "device_manager");
}

void TopWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasFormat("text/module")) {
        e->acceptProposedAction();
    }
}

void TopWindow::dropEvent(QDropEvent *e) {
    // if (e->mimeData()->hasFormat("text/module"))
    //     qDebug() << "text/module:" << e->mimeData()->data("text/module").data();

    if (e->mimeData()->hasFormat("text/device")) {
        auto list = split(e->mimeData()->data("text/device").data(), '\n');

        for (const auto &i : list) {
            /*
timer_ = new QTimer();
QObject::connect(timer_, &QTimer::timeout, this, &QFormScreen::onTimer);
timer_->setInterval(static_cast<int>(1000 * time_step_.toDouble()));
timer_->start(static_cast<int>(time_step_.toDouble() * 1000));
*/
            auto dock_widgets = findChildren<QDockWidget *>();
            auto title = QString::fromStdString(i);

            bool exists = false;
            for (auto dock : dock_widgets)
                if (dock->windowTitle() == title) {
                    exists = true;
                    break;
                }

            if (!exists) {
                auto dock = new QDockWidget(i.data(), this);

                QWidget *form_screen = new QFormScreen();
                dock->setWidget(form_screen);
                addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock, Qt::Orientation::Horizontal);
            }
        }
    }
}
