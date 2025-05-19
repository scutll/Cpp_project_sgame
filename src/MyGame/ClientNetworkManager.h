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
        Login = 0,FriendApplication,SendAcceptApplicationNotice,NormalMessage,
        /*client接收*/
        NoticeNewLogin, WaitAcceptApplication,  AcceptedApplication,
         SendNormalMesssage, SendUserDisconnected
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
    void sendUserNormalMessage(const QString& senderName,const QString& receiverName,const QString& message);

signals:
    void connectErrorSignal(const QString& error);
    void connectedSignal();
    void UserLogined(const QString& userName);
    void acceptNormalMessage(const QString& senderName,const QString& message);
    void userDisconnectedSignal(const QString& userName);


};



#endif //CLIENTNETWORKMANAGER_H
