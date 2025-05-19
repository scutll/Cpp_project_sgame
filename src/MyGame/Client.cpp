//
// Created by mxl_scut on 25-5-15.
//

#include "Client.h"


QString Client::userName_() const{
    return GLOB_UserName;
}

Client::Client(QWidget* parent) {
    Client(QString(__TIME__),0);
}


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
    connect(this->client_network_manager,&ClientNetworkManager::connectErrorSignal,this,&Client::dealconnectErrorSignal,Qt::QueuedConnection);

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

void Client::dealconnectErrorSignal(const QString &error) {
    emit INTERFACE_dealConnnectError(error);
    this->deleteChatThread();
}


void Client::deleteChatThread() {

    if (this->chat_thread != Q_NULLPTR) {
        this->client_network_manager->deleteLater();
        this->client_network_manager = Q_NULLPTR;

        this->chat_thread->exit();
        this->chat_thread->wait();
        this->chat_thread->deleteLater();
        this->chat_thread = Q_NULLPTR;

    }

}

void Client::setUserName(const QString &userName) {
    GLOB_UserName = userName;
    qDebug() << GLOB_UserName;
}


void Client::dealAcceptNormalMessage(const QString &senderName, const QString &message) {

    emit this->INTERFACE_dealAcceptNormalMessage(senderName,message);
}



void Client::dealUserDisconnected(const QString &userName) {

    emit this->INTERFACE_dealUserDisconnected(userName);
}

void Client::dealUserLogined(const QString& userName) {
    emit this->INTERFACE_dealUserLogined(userName);

}

void Client::INTERFACE_clientLogin(const QString &userName) {

    emit this->clientLogin(userName);

}

void Client::INTERFACE_SendUserMessage(const QString &senderName, const QString &receiverName, const QString &message) {

    emit this->SendUserMessage(senderName,receiverName,message);

}

void Client::INTERFACE_retryConnect() {


    this->deleteChatThread();

    this->chat_thread  = new QThread;
    this->client_network_manager = new ClientNetworkManager;
    this->client_network_manager->moveToThread(this->chat_thread);

    //线程启动
    connect(this->chat_thread,&QThread::started,this->client_network_manager,&ClientNetworkManager::initializeSocket,Qt::DirectConnection);
    this->chat_thread->start();


    //重新连接
    connect(this->client_network_manager,&ClientNetworkManager::connectErrorSignal,this,&Client::dealconnectErrorSignal,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::UserLogined,this,&Client::dealUserLogined,Qt::QueuedConnection);
    connect(this,&Client::SendUserMessage,this->client_network_manager,&ClientNetworkManager::sendUserNormalMessage,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::acceptNormalMessage,this,&Client::dealAcceptNormalMessage,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::userDisconnectedSignal,this,&Client::dealUserDisconnected,Qt::QueuedConnection);

}


