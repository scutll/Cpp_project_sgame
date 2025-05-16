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
    ~Client();

private:
    QThread* chat_thread = nullptr;
    ClientNetworkManager* client_network_manager = nullptr;

public:

    void deleteChatThread();
    void dealAcceptNormalMessage(const QString& senderName,const QString& message);
    void dealUserDisconnected(const QString& userName);

signals:
    void sendLoginInfo();

//提供给外部的接口
public:
    void dealUserLogined(const QString& userName);
signals:
    void SendUserMessage(const QString& senderName,const QString& receiverName,const QString& message);
};



#endif //CI