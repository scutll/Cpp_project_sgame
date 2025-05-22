//
// Created by mxl_scut on 25-5-15.
//

#ifndef CLIENTWORK_H
#define CLIENTWORK_H
#include <qtmetamacros.h>
#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QDataStream>
#include <QTcpSocket>
 
class ClientWork :public QObject{
    Q_OBJECT
    enum MSGTYPE {
        /*server接收*/
        LoginRequest = 0,FriendApplication,SendAcceptApplicationNotice,NormalMessage,
        /*client接收*/
        RefuseLoginWrongPassword,NoticeNewLogin, WaitAcceptApplication,  AcceptedApplication,
         SendNormalMesssage, SendUserDisconnected
    };

public:
    ClientWork(const quintptr& handle, QObject* parent = nullptr);
    ~ClientWork();
public:
    void initializeSocket();
    int SocketId();
    void dealNoticeClientDisconnected(const QString& userName);
    void noticeClientLogin(const qint64 userAccount,const QString& userName);
    void noticeRefusedWrongPsw(const qint64 userAccount);
    Q_INVOKABLE void sendUserMessageToReceiver(const QString& senderName, const QString& receiverName,const QString& message);
private:
    void ReadData();
    void dealClientDisconnected();
private:
    QTcpSocket* socket = nullptr;
    QString userName;
    qint64 userAccount = 0;
    QString receiverUserAccount_temp;
    QMutex mutex;
    qintptr socketDescriptor = 0;
    int socket_id = 0;
    quint16 nextBlockSize = 0;
    qint16 MSG_TYPE = -1;

signals:
    void NoticeClientDisconnected(const QString& userName);    //提醒服务器用户退出连接
    void updateLocalUserData(const QString& userName,const QString& type);
    void clientDisconnected(int socket_id);
    void newClientLogin(const qint64 userAccount,const QString& userPassword);
    void acceptUserNormalMessage(const QString& senderName,const QString& receiverName,const QString& message);


};



#endif //CLIENTWORK_H
