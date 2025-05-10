#ifndef PLAYER_CONNECT_H
#define PLAYER_CONNECT_H

#include <QDialog>
#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include "../../src/save_package.h"

class PlayerConnector : public QObject{
    Q_OBJECT

private:
    QTcpSocket* Server_socket;  //存储服务器socket对象
    QString ServerIP;
    int ServerPort;
    //主窗口的指针，用来通知主窗口刷新和发送信息

    QString new_msg;
    package new_pkg;

private slots:
    void recv_msg();

signals:
    void connectionSucceeded();
    void connectionFailed(const QString &errorMessage);

    void recv_msg_str(const QString& msg);
    void recv_msg_pakcage(const package& pkg);

private slots:
    void onConnected() {
        qDebug() << "connection succeed!";
        emit connectionSucceeded();
    }

    void onError(QAbstractSocket::SocketError socketError) {
        qDebug() << "failed to connect to the server!";
        emit connectionFailed("连接失败");
    }

    void onTimeout() {
        if (Server_socket->state() != QAbstractSocket::ConnectedState) {
            qDebug() << "connection timeout!";
            emit connectionFailed("连接超时");
        }
    }

public:
    PlayerConnector();
    void set_IP(const QString IP);
    void set_port(const int port);

    bool connect_to_server();

    void send_msg(const QString msg);


    void send_match_request();
    void send_complete_msg();
    void receive_game_map_package();
};

#endif // PLAYER_CONNECT_H
