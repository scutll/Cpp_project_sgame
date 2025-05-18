//
// Created by mxl_scut on 25-5-15.
//

#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QString>
#include <QTcpServer>
#include "ClientWork.h"
#include "TcpServer.h"
#include <QThread>
#include <QVector>



class ChatServer : public QObject{
    Q_OBJECT
public:
    ChatServer(QObject* parent = nullptr);
    ~ChatServer();
private:
    void newClientConnection(qintptr handle);
    void dealNoticeClientDisconnected(const QString& userAccount);
    void dealUpdateLocalUserData(const QString& userAccount,const QString& type);
    void dealClientDisconnected(int socket_id);
    void dealNewClientLogin(const QString& usrName);
    void dealAcceptUserNormalMessage(const QString& senderName,const QString& receiverName,const QString& message);

private:
    TcpServer *tcpServer;
    QMap<QThread*, ClientWork*> clients;
    QVector<QString> existingUsers;

signals:
    void transferNoticeDisconnected(const QString& userAccount);
    void transferNewClientLogin(const QString& userAccount);
    void transferAcceptUserNormalMessage(const QString& senderAccount, const QString& receiverAccount, const QString& msg);



};



#endif //CHATSERVER_H
