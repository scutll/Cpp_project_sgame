#include "mainwindow.h"
#include "start_window.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    start_window Start;
    MainWindow* game_window = Start.GameWindow();

    //绑定召回开始窗口
    MainWindow::connect(game_window,&MainWindow::Awake_StartWindow,&Start,&start_window::on_Awake_signal);
    Start.show();

    // Archives arc;
    // arc.show();
    return a.exec();
}
