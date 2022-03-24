#include "Receiver/Receiver.h"
//
#include <QtWidgets>
//
#include "Actions.h"
#include "GUI/TreeEditor.h"
#include "GUI/WidgetWrappers.h"
#include "Ping/ksdconnected.h"
#include "common/ExtensionManager.h"
#include "common/IO_ifs.h"
#include "common/StringProcessingTools.h"
#include "convtemplate/Parameter_ifs.h"
#include "convtemplate/PrmBuffer_ifs.h"
#include "device/Device.h"
#include "mainwindow.h"
#include "qformscreen.h"
class Parameter_ifs;

class OpenAction : public QAction {
   public:
    explicit OpenAction(ExtensionManager *manager, IO_ifs *io, QObject *parent = nullptr)
        : QAction(parent), manager_(manager), io_(io) {
        setText(QObject::tr("&Open") + " " + io->file_type_.c_str());
        //+ " " + io->file_type_.c_str()
        connect(this, &QAction::triggered, this, &OpenAction::openFile);
    }

   protected slots:
    void openFile() {
        auto f_name = QFileDialog::getOpenFileName(nullptr, QString(), QString(), io_->filename_pattern_.c_str(),
                                                   nullptr, QFileDialog::Options());
        io_->readDocument(manager_, f_name.toStdString());
    }

   protected:
    ExtensionManager *manager_ = nullptr;
    IO_ifs *io_;
};

ModuleStream_ifs *generateStream(ExtensionManager *manager, const std::string &type, Device *device,
                                 const std::list<Parameter_ifs *> &parameters) {
    ModuleStream_ifs *top_m_stream = device->createModuleStream();

    for (auto prm : parameters) {
        auto constructor = (prmBufferConstructor_f)manager->getLastVersionExtensionObject(type, prm->getType());
        if (constructor) {
            auto prm_buffer = constructor(prm, manager);

            auto full_path = prm->getProperty("common/path")->getValue().asString();

            auto path = lastCharPos(full_path, '/');
            auto sub_modules = ::getSubmodules(device, path.first);
            if (sub_modules.size() == 1) {
                auto m = sub_modules.front()->getModuleStream();
                if (m) top_m_stream->addPrmBuffer(path.second, prm_buffer);
            }
        }
    }

    return top_m_stream;
}

MainWindow::MainWindow(ExtensionManager *ctm) : text_edit_(new QTextEdit), manager_(ctm) {
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    ////////////////////////////////////////////////////////////////////////////
    /*
    std::string error_msg;

    EthernetAddress addr;
    addr.ip = inet_addr("192.168.001.176");
    addr.port = 0;

    auto devices = devicePing(addr, error_msg);

    if (!error_msg.empty()) {
        std::cout << error_msg;
        return;
    }
    auto v = devices.front()->getTask();

    Device *device = new Device(v.data(), v.size(), manager_);

    auto form_screen = new QFormScreen;

    ModuleStream_ifs *m_stream = device->getTopModule()->createModuleStream();
    auto *receiver = new Receiver(m_stream, device->getSrcAddress());

    auto prm_buff = device->getTopModule()->getPrmBufferMap();
    for (const auto &i : prm_buff) form_screen->addScale(getReaderExample(i.second));

    receiver->start();
     */
    //////////////////////////////////////////////////////////////////////////
    setCentralWidget(text_edit_);

    auto file_menu = menuBar()->addMenu(tr("&Файл"));

    auto io_units = manager_->getLastVersionExtensionUnitsByType("io");

    for (auto i : io_units) {
        if (i && i->ptr) {
            auto new_base = new OpenAction(manager_, (IO_ifs *)i->ptr, this);
            new_base->setStatusTip(tr("&Create a new file"));
            file_menu->addAction(new_base);
        }
    }

    createDockWindows();
    setWindowTitle(tr("&Express RealT"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::createDockWindows() {
    QDockWidget *dock = nullptr;

    /*
     *
     */

    // dock = new QDockWidget(tr("&parameter properties"), this);
    dock = new QDockWidget(tr("&свойства параметра"), this);
    dock->setObjectName(tr("&parameter properties"));
    TreeEditor *top = (TreeEditor *)manager_->getLastVersionExtensionUnit("tree_editor", "tree_editor")->ptr;

    dock->setWidget(top);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    this->resize(750, 600 * 2);
    top->setMinimumHeight(600);
    top->setMinimumWidth(300);
    top->setColumnWidth(0, 250);
    /*
     *
     */

    auto units = manager_->getLastVersionExtensionUnitsByType("widget");
    for (auto i : units) {
        dock = new QDockWidget(tr(i->name), this);
        qDebug() << i->name;
        auto widget = (QWidget *)i->ptr;  // ctm_->getLastVersionExtensionUint("widget", "conv_template_list")->ptr;
        dock->setObjectName(tr(i->name));
        dock->setWidget(widget);
        addDockWidget(Qt::BottomDockWidgetArea, dock);
    }

    units = manager_->getLastVersionExtensionUnitsByType("widget_wrapper");
    for (auto i : units) {
        dock = new QDockWidget(tr(i->name), this);
        qDebug() << i->name;
        auto widget_wrapper =
            (WidgetWrapper_ifs *)i->ptr;  // ctm_->getLastVersionExtensionUint("widget", "conv_template_list")->ptr;
        auto widget = widget_wrapper->getWidget();
        dock->setObjectName(tr(i->name));
        dock->setWidget(widget);
        addDockWidget(Qt::BottomDockWidgetArea, dock);
    }

    QSettings settings("none", "express_rt");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings("none", "express_rt");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QWidget::closeEvent(event);
}
//! [9]
