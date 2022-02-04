
#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#    include <QtPrintSupport/qtprintsupportglobal.h>
#    if QT_CONFIG(printdialog)
#        include <QtPrintSupport>
#    endif
#endif

#include "GUI/TreeEditor.h"
#include "common/ExtensionManager.h"
#include "convtemplate/Parameter_ifs.h"
#include "mainwindow.h"
class Parameter_ifs;

MainWindow::MainWindow(ExtensionManager *ctm) : text_edit_(new QTextEdit), ctm_(ctm) {
    // etStyle("windows");QWindowsVistaStyle
    qDebug() << QStyleFactory::keys();
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    setCentralWidget(text_edit_);

    createDockWindows();

    setWindowTitle(tr("Dock Widgets"));

    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::createDockWindows() {
    QDockWidget *dock = nullptr;

    /*
     *
     */

    dock = new QDockWidget(tr("parameter properties"), this);
    dock->setObjectName(tr("parameter properties"));
    auto constructor = (newTreeEditor_t)ctm_->getLastVersionExtensionUint("tree_editor", "tree_editor")->ptr;
    // auto ds = (DataSchema_ifs *)ctm_->getLastVersionExtensionUint("data_schema", "ethernet_udp")->ptr;

    typedef Parameter_ifs *(*create_parameter_t)(ExtensionManager * manager);
    auto prmConstructor = (create_parameter_t)(ctm_->getLastVersionExtensionUint("parameter", "EthernetUdp")->ptr);
    auto prm = prmConstructor(ctm_);

    qDebug() << toString(prm->getProperty(""), "").c_str();

    TreeEditor *top = constructor(ctm_, nullptr);
    top->setupProperties(prm);

    dock->setWidget(top);

    addDockWidget(Qt::RightDockWidgetArea, dock);

    this->resize(750, 600 * 2);
    top->setMinimumHeight(600);
    top->setMinimumWidth(300);
    top->setColumnWidth(0, 250);
    // dock->swtMinimumSizeHint(250, 900);
    /*
     *
     */

    auto units = ctm_->getLastVersionExtensionUintsByType("widget");
    for (auto i : units) {
        dock = new QDockWidget(tr(i->name), this);

        auto widget = (QWidget *)i->ptr;  // ctm_->getLastVersionExtensionUint("widget", "conv_template_list")->ptr;
        dock->setObjectName(tr(i->name));
        dock->setWidget(widget);
        addDockWidget(Qt::BottomDockWidgetArea, dock);
    }

    QSettings settings("MyCompany", "MyApp");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    // qDebug() << saveState().data();

    // qDebug() << settings.group();
    //  QMainWindow::closeEvent(event);
    /*
     *
     */
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings("MyCompany", "MyApp");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QWidget::closeEvent(event);
}
//! [9]
