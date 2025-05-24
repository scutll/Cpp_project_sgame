//
// Created by mxl_scut on 25-5-15.
//

#ifndef CLIENTNETWORKMANAGER_H
#define CLIENTNETWORKMANAGER_H
#include <qobject.h>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include "global.h"
#include <QObject>

class ClientNetworkManager :public QObject{
    Q_OBJECT
    enum MSGTYPE {
        /*server接收*/
        LoginRequest = 0,FriendApplication,SendAcceptApplicationNotice,NormalMessage,ModifyNameRequest,
        RegisterRequest,
        /*client接收*/
        RefuseLogin,NoticeNewLogin, AccountAlreadyLogined,
        WaitAcceptApplication,  AcceptedApplication,
        RepeatedNameRejected,UserNameModified,
        SendNormalMesssage, SendUserDisconnected,
        RegisterAccepted,AccountOccupied
    };
public:
    ClientNetworkManager(QObject* parent = nullptr);
    ~ClientNetworkManager();

private:
    quint16 nextBlockSize = 0;
    qint16 MSG_TYPE = -1;
    QTcpSocket* socket = nullptr;
    QMutex mutex;


public:
    void initializeSocket();
    void ReadData();
    void sendLoginInfo();
    void dealSendLoginRequest(const qint64 userAccount,const QString& userPassword);
    void sendUserNormalMessage(const QString& senderName,const QString& receiverName,const QString& message);
    void dealServerDisconnected();
    void dealNameModifyRequest(const qint64 userAccount,const QString& newName);
    bool isConnected();
    void dealRegisterRequest(const qint64 userAccount,const QString& userPassword,const QString& userName);


signals:
    void AccountAlreadyLoginedSignal(const qint64 userAccount);
    void NoticeAccountOccupied(const qint64 userAccount);
    void NoticeRegisterAccepted(const qint64 userAccount,const QString& userName,const QString& extName);
    void noticeRepeatedNameRejected(const qint64 userAccount);
    void noticeNameModified(const qint64 userAccount,const QString& userName,const QString& newName);
    void connectErrorSignal(const QString& error);
    void connectedSignal();
    void RefuseLoginSignal(const qint64 userAccount);
    void noticeUserLogined(const qint64 userAccount,const QString& userName);
    void acceptNormalMessage(const QString& senderName,const QString& message);
    void userDisconnectedSignal(const QString& userName);
    void ServerDisconnectedSignal();

};



#endif //CLIENTNETWORKMANAGER_H
