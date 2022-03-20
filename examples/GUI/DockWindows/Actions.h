//
// Created by SVK on 18.03.2022.
//

#ifndef EXRT_ACTIONS_H
#define EXRT_ACTIONS_H
#include <QAction>
#include <QtWidgets>

#include "common/ExtensionManager.h"

class RunAction : public QAction {
   public:
    explicit RunAction(ExtensionManager *manager, QObject *parent = nullptr) : QAction(parent), manager_(manager) {
        auto image_dir = QDir(QCoreApplication::applicationDirPath() + "/png/common");
        const QIcon run_icon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
        setIcon(run_icon);
        connect(this, &QAction::triggered, this, &RunAction::run);
        setStatusTip(tr("run"));
    }

   protected slots:
    void run() {}

   protected:
    ExtensionManager *manager_ = nullptr;
};

#endif  // EXRT_ACTIONS_H
