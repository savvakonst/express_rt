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
#include "qformscreen.h"

TopWindow::TopWindow(QWidget *parent) {
    setAcceptDrops(true);
    setWindowTitle(tr("MDI"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void TopWindow::init(ExtensionManager *extension_manager) {}

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
    // if (e->mimeData()->hasFormat("text/module"))
    //     qDebug() << "text/module:" << e->mimeData()->data("text/module").data();

    if (e->mimeData()->hasFormat("text/device")) {
        // qDebug() << "text/device:" << e->mimeData()->data("text/device").data();
        auto dock = new QDockWidget(e->mimeData()->data("text/device").data(), this);
        dock->setObjectName(QObject::tr("&plotter"));

        // QWidget *form_screen = !action_run_->isChecked() ? new QWidget : new QFormScreen();
        QWidget *form_screen = new QFormScreen();
        dock->setWidget(form_screen);
        addDockWidget(Qt::LeftDockWidgetArea, dock, Qt::Orientation::Horizontal);
    }
}

#ifndef PLOTTER_LIB_NAME
#    error "PLOTTER_LIB_NAME undefined"
#endif

static const version_t kPlotterVersion = 0;
static ExtensionUnit *g_plotter_extension_uint;
static ExtensionInfo g_plotter_extension_info;

static int initPlotter_(ExtensionManager *manager);


InitExtension(ExtensionInfo *) POST_CONCATENATOR(init, PLOTTER_LIB_NAME)(void) {
    if (QCoreApplication::instance() == nullptr) return nullptr;

    g_plotter_extension_uint = new ExtensionUnit[]{{"plotter", "plotter", "", new TopWindow(nullptr), 0x00},
                                                   {"plotter", "init", "", &initPlotter_, 0x00},
                                                   {nullptr, nullptr, nullptr, nullptr, 0}};

    g_plotter_extension_info = {"tree_widget_extension", 0x01, g_plotter_extension_uint};

    return &g_plotter_extension_info;
}


static int initPlotter_(ExtensionManager *manager) {
    auto p_unit = manager->getLastVersionExtensionUnit("plotter", "plotter");

    if (p_unit) {
        auto plotter = (TopWindow *)p_unit->ptr;
        if ((p_unit->version == kPlotterVersion) && plotter) {
            plotter->init(manager);
            return 0;
        }
    }

    return 1;
}

