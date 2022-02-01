

#include <QtWidgets/QApplication>
#include <iostream>

#include "GUI/TreeEditor.h"

InitExtension(ExtensionInfo *) initTreeEditor(void);

/*
 *
 *
 */

typedef std::vector<DataSchema_ifs *> bindList_t;

bindList_t getConfig();

std::string printer_debug(DataSchema_ifs *arg);

#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplateManager.h"
int main(int argc, char *argv[]) {
    // py::scoped_interpreter guard{};
    // py::module_ bind = py::module_::import("expressbind");

    QApplication a(argc, argv);

    ExtensionManager manager(false);
    manager.insertExtensionInfo(initTreeEditor());
    manager.insertExtensionUnit(new ExtensionUnit{"cnv_template_manager", "cnv_template_manager", "pass",
                                                  (void *)new ConversionTemplateManager(), 0x00});

    TreeEditor *top =
        ((newTreeEditor_t)manager.getLastVersionExtensionUint("tree_editor", "tree_editor")->ptr)(nullptr);
    top->addExtensionUint(&manager);

    manager.init();

    auto ds = (DataSchema_ifs *)manager.getLastVersionExtensionUint("data_schema", "ethernet")->ptr;
    ds->init(&manager);
    top->setupProperties(ds);

    top->show();
    top->resize(500, 600);
    top->setColumnWidth(0, 300);

    return a.exec();
}