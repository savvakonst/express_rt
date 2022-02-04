
#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#    include <QtPrintSupport/qtprintsupportglobal.h>
#    if QT_CONFIG(printdialog)
#        include <QtPrintSupport>
#    endif
#endif

#include "GUI/TreeEditor.h"
#include "common/ExtensionManager.h"
#include "common/IO_ifs.h"
#include "convtemplate/Parameter_ifs.h"
#include "mainwindow.h"

class Parameter_ifs;

class OpenAction : public QAction {
   public:
    explicit OpenAction(ExtensionManager *manager, IO_ifs *io, QObject *parent = nullptr)
        : QAction(parent), manager_(manager), io_(io) {
        setText(QObject::tr("&Open"));
        //+ " " + io->file_type_.c_str()
        connect(this, &QAction::triggered, this, &OpenAction::openFile);
    }

   protected slots:
    void openFile() {
        auto f_name =
            QFileDialog::getOpenFileName(nullptr, /*caption*/ QString(), /*dir */ QString(),
                                         /*filter*/ io_->filename_pattern_.c_str(), /*selectedFilter*/ nullptr,
                                         /*options*/ QFileDialog::Options());

        io_->readDocument(manager_, f_name.toStdString());
    }

   protected:
    ExtensionManager *manager_ = nullptr;
    IO_ifs *io_;
};

MainWindow::MainWindow(ExtensionManager *ctm) : text_edit_(new QTextEdit), manager_(ctm) {
    // etStyle("windows");QWindowsVistaStyle
    qDebug() << QStyleFactory::keys();
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    setCentralWidget(text_edit_);

    auto infoLabel =
        new QLabel(tr("<i>Choose a menu option, or right-click to "
                      "invoke a context menu</i>"));
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    auto file_menu = menuBar()->addMenu(tr("&File"));

    auto io_units = manager_->getLastVersionExtensionUintsByType("io");
    for (auto i : io_units) {
        if (i && i->ptr) {
            auto new_base = new OpenAction(manager_, (IO_ifs *)i->ptr, this);
            new_base->setStatusTip(tr("&Create a new file"));
            file_menu->addAction(new_base);
        }
    }

    createDockWindows();
    setWindowTitle(tr("&Dock Widgets"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::createDockWindows() {
    QDockWidget *dock = nullptr;

    /*
     *
     */

    dock = new QDockWidget(tr("&parameter properties"), this);
    dock->setObjectName(tr("&parameter properties"));
    auto constructor = (newTreeEditor_t)manager_->getLastVersionExtensionUint("tree_editor", "tree_editor")->ptr;
    // auto ds = (DataSchema_ifs *)ctm_->getLastVersionExtensionUint("data_schema", "ethernet_udp")->ptr;

    typedef Parameter_ifs *(*create_parameter_t)(ExtensionManager * manager);
    auto prmConstructor = (create_parameter_t)(manager_->getLastVersionExtensionUint("parameter", "EthernetUdp")->ptr);
    auto prm = prmConstructor(manager_);

    qDebug() << toString(prm->getProperty(""), "").c_str();

    TreeEditor *top = constructor(manager_, nullptr);
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

    auto units = manager_->getLastVersionExtensionUintsByType("widget");
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
