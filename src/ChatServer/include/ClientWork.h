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
#include <QTcpSocket>

class ClientWork :public QObject{
    Q_OBJECT
    enum MSGTYPE {
        Login = 0, NoticeNewLogin, FriendApplication, WaitAcceptApplication, SendAcceptApplicationNotice, AcceptedApplication,
        NormalMessage, SendNormalMesssage, SendUserDisconnected
    };

public:
    ClientWork(const quintptr& handle, QObject* parent = nullptr);
    ~ClientWork();
public:
    void initalizeSocket();
    int SocketId();
    void dealNoticeClientDisconnected(const QString& UserAccount);
    void noticeClientLogin(const QString& userAccount);
    Q_INVOKABLE void sendUserMessageToReceiver(const QString& senderAccount,const QString& msg);
private:
    void ReadData();
    void dealClientDisconnected();
private:
    QTcpSocket* socket = nullptr;
    QString userAccount;
    QString receiverUserAccount_temp;
    QMutex mutex;
    qintptr socketDescriptor = 0;
    int socket_id = 0;

signals:
    void NoticeClientDisconnected(const QString& userAccount);    //提醒服务器用户退出连接
    void updateLocalUserData(const QString& userAccount,const QString& type);
    void clientDisconnected(int socket_id);
    void newClientLogin(const QString& userAccount);
    void acceptUserNormalMessage(const QString& senderAccount,const QString& receiverAccount,const QString& msg);


};



#endif //CLIENTWORK_H
