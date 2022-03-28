//
// Created by SVK on 27.03.2022.
//

#ifndef EXRT_TOPWINDOW_H
#define EXRT_TOPWINDOW_H

#include <QMainWindow>

#include "common/ExtensionManager.h"

class TopWindow : public QMainWindow {
   public:
    explicit TopWindow( QWidget* parent = nullptr);

    void init(ExtensionManager* extension_manager);

    void dragEnterEvent(QDragEnterEvent* e) override;
    void dropEvent(QDropEvent* e) override;

    // void dragMoveEvent(QDragMoveEvent* event) override;
    // void dragLeaveEvent(QDragLeaveEvent* event) override;
};








#endif  // EXRT_TOPWINDOW_H
