//
// Created by mxl_scut on 25-5-15.
//

#include "../include/Client.h"

Client::Client(const QString &userName, const int &userAccount, QWidget *parent)
    :QWidget(parent){
    GLOB_UserAccount = userAccount;
    GLOB_UserName = userName;

    this->chat_thread  = new QThread;
    this->client_network_manager = new ClientNetworkManager;
    this->client_network_manager->moveToThread(this->chat_thread);

    //线程启动
    connect(this->chat_thread,&QThread::started,this->client_network_manager,&ClientNetworkManager::initializeSocket,Qt::DirectConnection);
    this->chat_thread->start();


    //连接错误
    connect(this->client_network_manager,&ClientNetworkManager::connectErrorSignal,this,&Client::deleteChatThread,Qt::QueuedConnection);

    //服务器通知有用户登录
    connect(this->client_network_manager,&ClientNetworkManager::UserLogined,this,&Client::dealUserLogined,Qt::QueuedConnection);

    //前端提出发送信息请求，转移到工作线程发送
    connect(this,&Client::SendUserMessage,this->client_network_manager,&ClientNetworkManager::sendUserNormalMessage,Qt::QueuedConnection);

    //收到消息，进行处理
    connect(this->client_network_manager,&ClientNetworkManager::acceptNormalMessage,this,&Client::dealAcceptNormalMessage,Qt::QueuedConnection);

    //服务器通知用户有其他用户离线
    connect(this->client_network_manager,&ClientNetworkManager::userDisconnectedSignal,this,&Client::dealUserDisconnected,Qt::QueuedConnection);

}

Client::~Client() {
    this->deleteChatThread();
}

