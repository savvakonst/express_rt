#include "qformscreen.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>


#include "Device/Device.h"
#include "Receiver/Receiver.h"
#include "common/extension.h"
#include "Ping/ksdconnected.h"
#include <winsock2.h>
bool initWSA() ;

class DeviceBuildingContext : public DeviceBuildingContext_ifs {
public:
    ~DeviceBuildingContext() {}

    bool addUint(ExtensionUint *uint) {
        if (std::string(uint->type) == "module") map_[uint->name] = (moduleConstructor_f)uint->ptr;
        return true;
    }

    moduleConstructor_f getConstructor(const std::string &module_id) override {
        auto e = map_.find(module_id);
        if (e == map_.end()) {
            return nullptr;
        } else {
            return e->second;
        }
    }

    Module_ifs *createModule(const std::string &module_id, const void *ptr, size_t size,
                             DeviceBuildingContext_ifs *context) override {
        moduleConstructor_f constructor = context->getConstructor(module_id);
        if (constructor) return constructor(ptr, size, context);
        return nullptr;
    }

private:
    std::map<std::string, moduleConstructor_f> map_;
};

InitExtension(ExtensionInfo *) initModules(void);

#include <iostream>

int main(int argc, char* argv[]){
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QFormScreen w;

    //auto prm_buffer_1 = createNewPrmBufferSimulator(16, 1 << 12, {0, 30}, (0.1 * (2 - 0)));
    //auto prm_buffer_2 = createNewPrmBufferSimulator(16, 1 << 12, {0, 30}, (0.1 * (2 - 1)));


    initWSA();

    ExtensionInfo *extension_info = initModules();
    auto uints = extension_info->uints;

    DeviceBuildingContext building_context;

    while (uints->name) {
        building_context.addUint(uints++);
    }

    std::string error_msg;
    auto devices = devicePing(error_msg);

    if (error_msg.size()) {
        std::cout << error_msg;
        return 1;
    }
    auto v = devices.front()->getTask();

    Device device = Device(v.data(), v.size(), &building_context);

    //if (device.hasError()) std::cout << "error: " << device.getErrorMessage() + "\n";
    if (device.hasTransceiver()) {
        ModuleStream_ifs *m_stream = device.getTopModule()->createModuleStream();
        Receiver receiver = Receiver(m_stream, device.getSrcAddress());

        auto prm_buff = device.getTopModule()->getPrmBufferMap();
        for(auto i:prm_buff){
            w.addScale(getReaderExample(i.second));
        }
        receiver.start();
        //w.addScale(getReaderExample(prm_buffer_1));
        //w.addScale(getReaderExample(prm_buffer_2));
        w.show();
        return a.exec();
    }
    //return 0;



    //w.addScale(getReaderExample(prm_buffer_1));
    //w.addScale(getReaderExample(prm_buffer_2));
    //w.show();
    return a.exec();
}
