#include "mainwindow.h"
#include "start_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    start_window Start;
    MainWindow w;
    Start.show();

    return a.exec();
}
