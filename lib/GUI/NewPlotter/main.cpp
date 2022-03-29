#include "Receiver/Receiver.h"
//
#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include <iostream>

#include "Ping/ksdconnected.h"
#include "common/Extension.h"
#include "common/ExtensionManager.h"
#include "device/Device.h"
#include "qformscreen.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator qt_translator;
    qt_translator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(&qt_translator);

    auto manager = new ExtensionManager;

    std::string error_msg;

    EthernetAddress addr;
    addr.ip = inet_addr("192.168.001.176");
    addr.port = 0;

    auto devices = devicePing(addr, error_msg);

    if (!error_msg.empty()) {
        std::cout << error_msg;
        return 0;
    }
    auto v = devices.front()->getTask();

    Device device = Device(v.data(), v.size(), manager);

    /////////////////////////////////////

    if (device.hasTransceiver()) {
        QFormScreen w(nullptr);
        ModuleStream_ifs *m_stream = device.getTopModule()->createModuleStream();
        auto *receiver = new Receiver(m_stream, device.getSrcAddress());

        auto prm_buff = device.getTopModule()->getPrmBufferMap();
        for (const auto &i : prm_buff) w.addScale(getReaderExample(i.second));

        receiver->start();
        w.show();
        return QApplication::exec();
    }

    return QApplication::exec();
}
/*
 *
 *
 *
 *
 *
 */

#include "TopWindow.h"

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