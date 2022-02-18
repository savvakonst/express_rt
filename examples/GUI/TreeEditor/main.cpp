

#include <QtWidgets/QApplication>

#include "GUI/TreeEditor.h"
#include "common/ExtensionManager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    ExtensionManager manager;

    TreeEditor *top = ((TreeEditor *)manager.getLastVersionExtensionUnit("tree_editor", "tree_editor")->ptr);

    typedef Parameter_ifs *(*create_parameter_t)(ExtensionManager * manager);
    auto constr = (create_parameter_t)(manager.getLastVersionExtensionUnit("parameter", "EthernetUdp")->ptr);
    auto prm = constr(&manager);
    top->setupProperties(prm);

    top->show();
    top->resize(500, 600);
    top->setColumnWidth(0, 300);

    return a.exec();
}