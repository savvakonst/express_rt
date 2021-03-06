
#ifndef EXRT_MAINWINDOW_H
#define EXRT_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
class QTableView;
class ConversionTemplateManager;
class ExtensionManager;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(ExtensionManager *manager);

   private:
    void closeEvent(QCloseEvent *event) override;

    void createDockWindows();

    ExtensionManager *manager_ = nullptr;
    QMenu *view_menu_ = nullptr;
};

#endif
