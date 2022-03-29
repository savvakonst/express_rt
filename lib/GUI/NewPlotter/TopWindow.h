//
// Created by SVK on 27.03.2022.
//

#ifndef EXRT_TOPWINDOW_H
#define EXRT_TOPWINDOW_H

#include <QMainWindow>

#include "common/ExtensionManager.h"

class DeviceManager;

class TopWindow : public QMainWindow {
   public:
    explicit TopWindow(QWidget* parent = nullptr);

    void init(ExtensionManager* manager);

    void dragEnterEvent(QDragEnterEvent* e) override;
    void dropEvent(QDropEvent* e) override;

   private:
    DeviceManager* device_manager_ = nullptr;
    // void dragMoveEvent(QDragMoveEvent* event) override;
    // void dragLeaveEvent(QDragLeaveEvent* event) override;

    ExtensionManager *manager_ = nullptr;
};

#endif  // EXRT_TOPWINDOW_H
