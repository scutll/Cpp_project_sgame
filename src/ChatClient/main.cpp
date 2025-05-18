#include "widget.h"

#include <QApplication>
#include <cstdlib>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    QString name = std::rand() % 10 > 5 ? "Tom" : "Amy";

    w.client = new Client(name,0);
    w.app_message("系统","welcome! " + w.client->userName_(),false);
    w.init_client();
    return a.exec();
}
