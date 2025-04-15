#include "mainwindow.h"
#include "start_window.h"
#include <QApplication>
#include "archives.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    start_window Start;
    MainWindow w;
    Start.show();

    // Archives arc;
    // arc.show();
    return a.exec();
}
