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
    void SaveUserInfo(const QString& userName,const qint64 userAccount);
    void SaveUserPsw(const qint64 userAccount, const QString& password);
private:
    void newClientConnection(qintptr handle);
    void dealNoticeClientDisconnected(const QString& userName);
    void dealUpdateLocalUserData(const QString& userName,const QString& type);
    void dealClientDisconnected(int socket_id);
    void dealNewClientLogin(const QString& usrName);
    void dealAcceptUserNormalMessage(const QString& senderName,const QString& receiverName,const QString& message);

private:
    TcpServer *tcpServer;
    QMap<QThread*, ClientWork*> clients;
    QVector<QString> existingUsers;
    QString UserInfoPath;
    QString UserPasswordPath;

signals:
    void transferNoticeDisconnected(const QString& userName);
    void transferNewClientLogin(const QString& userName);
    void transferAcceptUserNormalMessage(const QString& SenderUserName, const QString& receiverUserName, const QString& msg);



};



#endif //CHATSERVER_H
