#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;

    QCoreApplication::setOrganizationName("Krzysztof Gorecki");
    QCoreApplication::setApplicationName("pandora commander");

    window.show();

    return a.exec();
}
