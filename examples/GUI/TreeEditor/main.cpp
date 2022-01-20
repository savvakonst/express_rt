

#include <QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <iostream>

#include "TreeEditor.h"

static ExtensionUnit g_tree_widget_extension_uint[] = {
    {"large_text", "tree_widget_wrapper", "return wrapper of TreeTextEdit", (void *)newTreeWW<TreeTextEdit>, 0x00},
    {"bool", "tree_widget_wrapper", "return wrapper of TreeCheckBox", (void *)newTreeWW<TreeCheckBox>, 0x00},
    {"text", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWW<TreeLineEdit>, 0x00},
    {"ip", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWW<TreeIPEdit>, 0x00},
    {"number", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWW<TreeLineEdit>, 0x00},
    {"enum", "tree_widget_wrapper", "return wrapper of TreeLineEdit", (void *)newTreeWW<TreeComboBox>, 0x00},
    {nullptr, nullptr, nullptr, nullptr, 0}};

static ExtensionInfo g_tree_widget_extension_info = {"tree_widget_extension", 0x01, g_tree_widget_extension_uint};

InitExtension(ExtensionInfo *) initModules(void) { return &g_tree_widget_extension_info; }

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
    TreeEditor top = TreeEditor(ds_list[1]);

    ExtensionUnit *uint = g_tree_widget_extension_uint;
    while (uint->name) {
        top.addExtensionUint(uint++);
    }
    top.setupProperties();
    top.show();
    top.resize(500, 600);
    top.setColumnWidth(0, 300);

    return a.exec();
}