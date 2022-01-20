

#include <QtWidgets/QApplication>

#include "TopWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    TopWindow window;

    window.show();

    return a.exec();
}