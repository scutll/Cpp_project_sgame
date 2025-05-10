#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QDialog>
#include "mainwindow.h"
#include "player_connect.h"
#include <QStandardItemModel>

namespace Ui {
class start_window;
}

class start_window : public QDialog
{
    Q_OBJECT

public:
    explicit start_window(QWidget *parent = nullptr);
    ~start_window();

public:
    void add_msg(const QString msg,bool error = false);
    void msg_server(const QString msg);
    void msg_os(const QString msg,bool error = false);

    void on_Awake_signal();
    MainWindow* GameWindow(){
        return w;
    }

private slots:
    void on_offline_game_clicked();

    void on_online_game_clicked();

    void onConnectionFailed(const QString &errorMessage);

    void onConnectionSucceeded();

    void onRecv_msg_str(const QString& msg);

    void onRecv_msg_package(const package& pkg);

private:
    bool matching;
    Ui::start_window *ui;
    QStandardItemModel *model;
    MainWindow* w;
    PlayerConnector* player;
};

#endif // START_WINDOW_H
