#ifndef PLAYER_CONNECT_H
#define PLAYER_CONNECT_H

#include <QDialog>
#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include "../../src/save_package.h"

class PlayerConnector : public QObject{
private:
    QTcpSocket* Server_socket;  //存储服务器socket对象
    QString ServerIP;
    int ServerPort;
    //主窗口的指针，用来通知主窗口刷新和发送信息

    QString new_msg;
    package new_pkg;

public:
    PlayerConnector();
    void set_IP(const QString IP);
    void set_port(const int port);

    bool connect_to_server();

    void send_msg(const QString msg);
    void recv_msg();


    void send_match_request();
    void send_complete_msg();
    void receive_game_map_package();
};

#endif // PLAYER_CONNECT_H
