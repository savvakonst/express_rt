

#include <QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <iostream>

#include "GUI/TreeEditor.h"


InitExtension(ExtensionInfo *) initTreeEditor(void) ;


/*
 *
 *
 */

typedef std::vector<DataSchema_ifs *> bindList_t;

bindList_t getConfig();

std::string printer_debug(DataSchema_ifs *arg);

int main(int argc, char *argv[]) {
    // py::scoped_interpreter guard{};
    // py::module_ bind = py::module_::import("expressbind");

    QApplication a(argc, argv);

    std::vector<DataSchema_ifs *> ds_list = getConfig();

    printer_debug(ds_list.front());
    TreeEditor *top = nullptr;

    auto x = initTreeEditor();

    ExtensionUnit *uint = x->units;
    while (uint->name) {
        if (std::string(uint->type) == "tree_editor")
            top = ((newTreeEditor_t)uint->ptr)(nullptr);
        uint++;
    }


    if (top == nullptr)
        std::cout<<"----------------\n";


    uint = x->units;
    while (uint->name) {
        top->addExtensionUint(uint++);
    }

    top->setupProperties(ds_list[1]);
    top->show();
    top->resize(500, 600);
    top->setColumnWidth(0, 300);

    return a.exec();
}