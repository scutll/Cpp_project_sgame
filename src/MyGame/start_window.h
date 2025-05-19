#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include "Client.h"
#include <QPropertyAnimation>
#include "mainwindow.h"
#include "player_connect.h"
#include <QStandardItemModel>
#include "../ChatClient/Client.h"
#include "expandablemenu.h"
#include <QMenu>
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
    void app_msg(const QString& sender, const QString& message,bool error = false);
    void msg_server(const QString msg);
    void msg_os(const QString msg,bool error = false);
    void init_chatclient();

    void on_Awake_signal();
    MainWindow* GameWindow(){
        return w;
    }

    // void dealReceiverSet(const QString& receiverName);

private slots:
    void on_offline_game_clicked();

    void on_online_game_clicked();

    void onConnectionFailed(const QString &errorMessage);

    void onConnectionSucceeded();

    void onRecv_msg_str(const QString& msg);

    void onRecv_msg_package(const package& pkg);

    void gameClosed();

    // void on_ReceiverMenuBtn_clicked();

    void on_SendBtn_clicked();

    void on_LoginBtn_clicked();

    void on_RetryConnectionBtn_clicked();

    void dealINTERFACE_dealAcceptNormalMessage(const QString& senderName,const QString& message);

    void dealINTERFACE_dealUserDisconnected(const QString& userName);

    void dealINTERFACE_dealUserLogined(const QString& userName);

    void dealINTERFACE_dealConnnectError(const QString& error);


private:
    bool matching;
    Ui::start_window *ui;
    QStandardItemModel *model;
    MainWindow* w;
    PlayerConnector* GameServer;
    QMenu* menu;

    bool clientConnected = false;
    Client* chatclient;
    QString receiverName_temp = "All";

    bool menuON = false;
    bool userLoged = false;
    QString userName = Q_NULLPTR;


};

#endif // START_WINDOW_H
