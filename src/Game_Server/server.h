#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <vector>
#include <queue>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QMutex>
#include <QThread>
#include "../../src/save_package.h"
#include "../../src/MyGame/scene.h"

//服务器类
class Server : public QMainWindow
{
    Q_OBJECT
private:
    QString serverIP;
    int serverPort;
    QTcpServer* server;
    void recv_msg();

    quint16 nextBlockSize;
public:
    std::vector<std::pair<QTcpSocket*,QTcpSocket*>> compating_players_list;
    std::queue<QTcpSocket*> matching_players_list;
    mutable QMutex queuemutex;

private slots:
    void handle_new_connection();

    void handle_match_request();


public:
    void run();
    Server();
    void send_msg(QTcpSocket* player,const QString msg);
    void send_msg(QTcpSocket* player,const package pkg);
    void send_package_randomgame(QTcpSocket* player);
    ~Server();


    void match_two(QTcpSocket* lhs,QTcpSocket* rhs);
};

//继承于QTcpSocket类的客户类来处理单个连接
class ClientSocket : public QTcpSocket{
    Q_OBJECT

public:
    ClientSocket(QObject* parent = nullptr);

private slots:
    void recv_msg();

private:
    quint16 nextBlockSize;


};



#endif // SERVER_H