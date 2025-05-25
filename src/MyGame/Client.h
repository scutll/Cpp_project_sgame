//
// Created by mxl_scut on 25-5-15.
//

//Client类提供了处理用户登录和请求发送信息的接口，作为可拓展性类，其他类只需要调用相应接口就可以接入Cient类来获取客户端功能
#ifndef CILENT_H
#define CILENT_H
#include <QtWidgets/QWidget>
#include <QWidget>
#include "ClientNetworkManager.h"
#include "global.h"

class Client : public QWidget{
    Q_OBJECT

public:
    Client(const QString& userName,const qint64& userAccount,QWidget* parent = nullptr);
    Client(QWidget* parent = nullptr);
    ~Client();

private:
    QThread* chat_thread = nullptr;
    ClientNetworkManager* client_network_manager = nullptr;
    bool userLogined = false;

public:
    void setUserName(const QString& userName);
    QString userName_() const;
    void deleteChatThread();

signals:
    void sendLoginInfo();

    signals:
    void SendUserMessage(const QString& senderName,const QString& receiverName,const QString& message);
    void SendLoginRequest(const qint64 userAccount,const QString& userPassword);
    void NameModifyRequest(const qint64 userAccount,const QString& newName);
    void RegisterRequest(const qint64 userAccount,const QString& userPassword,const QString& userName);
public:
    void dealAcceptNormalMessage(const QString& senderName,const QString& message, const QString& sendTime);
    void dealUserDisconnected(const QString& userName);
    void dealNoticeUserLogined(const qint64 userAccount,const QString& userName);
    void dealAccountAlreadyLogined(const qint64 userAccount);
    void dealconnectErrorSignal(const QString& error);
    void dealServerDisconnected();
    void dealRefuseLoginSignal(const qint64 userAccount);
    void dealNoticeNameModified(const qint64 userAccount,const QString& userName,const QString& newName);
    void dealNoticeRepeatedNameRejected(const qint64 userAccount);
    void dealNoticeAccountOccupied(const qint64 userAccount);
    void dealNoticeRegisterAccepted(const qint64 userAccount,const QString& userName,const QString& extName);

    //构建CNM和Client之间的沟通
    void connect_Init();


    //提供给外部的接口
    //怎么处理数据，Client根本无需关心，它负责的只有把数据传输给本地类和接收本地类的信息并发送
    //public直接调用(Local的请求)，signals通过连接调用 (Client的通知)
public:
    void INTERFACE_registerRequest(const qint64 userAccount,const QString& userPassword,const QString& userName);
    void INTERFACE_retryConnect(const qint64 userAccount,const QString& userPassword);
    void INTERFACE_SendUserMessage(const QString& senderName,const QString& receiverName,const QString& message);
    void INTERFACE_LoginRequest(const qint64 userAccount,const QString& userPassword);
    bool INTREFACE_isConnected();
    void INTERFACE_UserNameModifyRequest(const qint64 userAccount,const QString& newName);

signals:
    void INTERFACE_NoticeAccountOccupied(const qint64 userAccount);
    void INTERFACE_NoticeRegisterAccepted(const qint64 userAccount,const QString& userName,const QString& extName);
    void INTERFACE_dealUserDisconnected(const QString& userName);
    void INTERFACE_dealUserLogined(const QString& userName);
    void INTERFACE_AccountAlreadyLogined(const qint64 userAccount);
    void INTERFACE_dealAcceptNormalMessage(const QString& senderName,const QString& message, const QString& sendTime);
    void INTERFACE_dealConnnectError(const QString &error);
    void INTERFACE_ServerDisconnected();
    void INTERFACE_LoginAccepted(const qint64 userAccount,const QString& userName);
    void INTERFACE_RefusedLogin(const qint64 userAccount);
    void INTERFACE_repeatedName();
    void INTERFACE_NoticeUserNameModified(const qint64 userAccount,const QString& userName,const QString& newName);
    void INTERFACE_NameModifyAccepted(const qint64 userAccount,const QString& newName);
};



#endif //CI
