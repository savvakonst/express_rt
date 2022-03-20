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

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(&qtTranslator);

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
        QFormScreen w;
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
