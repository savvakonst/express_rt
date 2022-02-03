

#include <QtWidgets/QApplication>

#include "GUI/TreeEditor.h"
#include "common/ExtensionManager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    ExtensionManager manager;

    TreeEditor *top =
        ((newTreeEditor_t)manager.getLastVersionExtensionUint("tree_editor", "tree_editor")->ptr)(&manager, nullptr);

    // auto ds = (DataSchema_ifs *)manager.getLastVersionExtensionUint("data_schema", "ethernet_udp")->ptr;
    auto ds = (DataSchema_ifs *)manager.getLastVersionExtensionUint("data_schema", "acceleration")->ptr;
    ds->init(&manager);
    top->setupProperties(ds);

    top->show();
    top->resize(500, 600);
    top->setColumnWidth(0, 300);

    return a.exec();
}