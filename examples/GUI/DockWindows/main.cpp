
#include <QApplication>
#include <QTranslator>
#include <fstream>
//

#include "common/ExtensionManager.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTranslator my_translator;
    my_translator.load("dictionary_ru");
    QApplication::installTranslator(&my_translator);

    ExtensionManager manager(false);

    MainWindow main_win(&manager);
    main_win.show();
    return QApplication::exec();
}
