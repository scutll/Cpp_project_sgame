#include "server.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server w;
    w.run();
    return a.exec();
}
