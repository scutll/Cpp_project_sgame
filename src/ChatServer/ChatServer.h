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
    bool checkLoginInfo(const qint64 userAccount,const QString& userPassword);
    QString getUserName(const qint64 userAccount);
    bool nameExists(const QString& userName);
    bool accountOccupied(const qint64 userAccount);

private:
    void newClientConnection(qintptr handle);
    void dealNoticeClientDisconnected(const QString& userName);
    void dealUpdateLocalUserData(const QString& userName,const QString& type);
    void dealClientDisconnected(int socket_id);
    void dealNewClientLogin(const qint64 userAccount,const QString& userPassword);
    void dealAcceptUserNormalMessage(const QString& senderName,const QString& receiverName,const QString& message);
    void dealModifyName(const qint64 userAccount,const QString& newName);
    void updateUserName(const qint64 userAccount,const QString& newName);
    void dealRegisterRequest(const qint64 userAccount,const QString& userPassword,const QString& userName);
private:
    TcpServer *tcpServer;
    QMap<QThread*, ClientWork*> clients;
    QVector<QString> existingUsers;     //这个暂时没有用，只是查看现在用户，但用户改名后并不会修改里面的旧名字
    QString UserInfoPath;
    QString UserPasswordPath;

signals:
    void transferNoticeDisconnected(const QString& userName);
    void transferNewClientLogin(const qint64 userAccount,const QString& userName);
    void transferAcceptUserNormalMessage(const QString& SenderUserName, const QString& receiverUserName, const QString& msg);
    void transferRefuseLogin(const qint64 userAccount);
    void transferRejectRepeatedName(const qint64 userAccount);
    void transferUserNameModified(const qint64 userAccount,const QString& userName,const QString& newName);
    void transferAccountOccupied(const qint64 userAccount);
    void transferRegisterAccepted(const qint64 userAccount,const QString& userName,const QString& extName);
};



#endif //CHATSERVER_H
