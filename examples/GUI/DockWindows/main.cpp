
#include <QApplication>
#include <QTranslator>
#include <fstream>
//
#include "common/BindingUtils.h"
#include "common/ExtensionManager.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTranslator my_translator;
    my_translator.load("dictionary_ru");
    app.installTranslator(&my_translator);

    ExtensionManager manager;

    auto evalFile = (evalFile_t)manager.getLastVersionExtensionObject("eval_file", "py_eval_file");
    if (evalFile) evalFile("exrt_config.py");

    MainWindow main_win(&manager);
    main_win.show();
    return QApplication::exec();
}
