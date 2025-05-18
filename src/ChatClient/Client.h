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
    Client(const QString& userName,const int& userAccount,QWidget* parent = nullptr);
    Client(QWidget* parent = nullptr);
    ~Client();

private:
    QThread* chat_thread = nullptr;
    ClientNetworkManager* client_network_manager = nullptr;

public:
    void setUserName(const QString& userName);
    QString userName_() const;
    void deleteChatThread();

signals:
    void sendLoginInfo();

    signals:
    void SendUserMessage(const QString& senderName,const QString& receiverName,const QString& message);
    void clientLogin(const QString& UserName);
public:
    void dealAcceptNormalMessage(const QString& senderName,const QString& message);
    void dealUserDisconnected(const QString& userName);
    void dealUserLogined(const QString& userName);


    //提供给外部的接口
    //怎么处理数据，Client根本无需关心，它负责的只有把数据传输给本地类和接收本地类的信息并发送
    //public直接调用(Local的请求)，signals通过连接调用 (Client的通知)
public:
    void INTERFACE_retryConnect();
    void INTERFACE_SendUserMessage(const QString& senderName,const QString& receiverName,const QString& message);
    void INTERFACE_clientLogin(const QString& userName);
signals:
    void INTERFACE_dealAcceptNormalMessage(const QString& senderName,const QString& message);
    void INTERFACE_dealUserDisconnected(const QString& userName);
    void INTERFACE_dealUserLogined(const QString& userName);
};



#endif //CI
