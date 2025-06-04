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


Client::Client(const QString &userName, const qint64 &userAccount, QWidget *parent)
    :QWidget(parent){
    GLOB_UserAccount = userAccount;
    GLOB_UserName = userName;

    qDebug() << userAccount << userName;
    this->chat_thread  = new QThread;
    this->client_network_manager = new ClientNetworkManager;
    this->client_network_manager->moveToThread(this->chat_thread);

    //线程启动
    connect(this->chat_thread,&QThread::started,this->client_network_manager,&ClientNetworkManager::initializeSocket,Qt::DirectConnection);
    this->chat_thread->start();


    this->connect_Init();

}

Client::~Client() {
    this->deleteChatThread();
}

void Client::dealconnectErrorSignal(const QString &error) {
    emit INTERFACE_dealConnnectError(error);
    this->deleteChatThread();
}

void Client::connect_Init() {

    //请求注册
    connect(this,&Client::RegisterRequest,this->client_network_manager,&ClientNetworkManager::dealRegisterRequest,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::NoticeAccountOccupied,this,&Client::dealNoticeAccountOccupied,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::NoticeRegisterAccepted,this,&Client::dealNoticeRegisterAccepted,Qt::QueuedConnection);

    //请求登录
    connect(this,&Client::SendLoginRequest,this->client_network_manager,&ClientNetworkManager::dealSendLoginRequest,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::RefuseLoginSignal,this,&Client::dealRefuseLoginSignal,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::AccountAlreadyLoginedSignal,this,&Client::dealAccountAlreadyLogined,Qt::QueuedConnection);

    //请求修改用户名
    connect(this,&Client::NameModifyRequest,this->client_network_manager,&ClientNetworkManager::dealNameModifyRequest,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::noticeNameModified,this,&Client::dealNoticeNameModified,Qt::QueuedConnection);
    connect(this->client_network_manager,&ClientNetworkManager::noticeRepeatedNameRejected,this,&Client::dealNoticeRepeatedNameRejected,Qt::QueuedConnection);
    //连接错误
    connect(this->client_network_manager,&ClientNetworkManager::connectErrorSignal,this,&Client::dealconnectErrorSignal,Qt::QueuedConnection);

    //服务器通知有用户登录
    connect(this->client_network_manager,&ClientNetworkManager::noticeUserLogined,this,&Client::dealNoticeUserLogined,Qt::QueuedConnection);

    //前端提出发送信息请求，转移到工作线程发送
    connect(this,&Client::SendUserMessage,this->client_network_manager,&ClientNetworkManager::sendUserNormalMessage,Qt::QueuedConnection);

    //收到消息，进行处理
    connect(this->client_network_manager,&ClientNetworkManager::acceptNormalMessage,this,&Client::dealAcceptNormalMessage,Qt::QueuedConnection);

    //服务器通知用户有其他用户离线
    connect(this->client_network_manager,&ClientNetworkManager::userDisconnectedSignal,this,&Client::dealUserDisconnected,Qt::QueuedConnection);

    //服务器断连消息
    connect(this->client_network_manager,&ClientNetworkManager::ServerDisconnectedSignal,this,&Client::dealServerDisconnected);
}

void Client::dealServerDisconnected() {
    qDebug() << "Server disconnected";
    emit this->INTERFACE_ServerDisconnected();
}




void Client::deleteChatThread() {

    qDebug() << "删除聊天线程中:";
    if (this->chat_thread != Q_NULLPTR) {
        this->client_network_manager->deleteLater();
        this->client_network_manager = Q_NULLPTR;

        this->chat_thread->exit();
        this->chat_thread->wait();
        this->chat_thread->deleteLater();
        this->chat_thread = Q_NULLPTR;

    }

}

bool Client::INTREFACE_isConnected() {
    return client_network_manager->isConnected();
}

void Client::setUserName(const QString &userName) {
    GLOB_UserName = userName;
    qDebug() << GLOB_UserName;
}


void Client::dealAcceptNormalMessage(const QString &senderName, const QString &message, const QString& sendTime) {

    emit this->INTERFACE_dealAcceptNormalMessage(senderName,message, sendTime);
}



void Client::dealUserDisconnected(const QString &userName) {

    emit this->INTERFACE_dealUserDisconnected(userName);
}

void Client::dealRefuseLoginSignal(const qint64 userAccount) {
    if (GLOB_UserAccount != userAccount) {
        return;
    }
    emit this->INTERFACE_RefusedLogin(userAccount);
}

void Client::dealNoticeUserLogined(const qint64 userAccount,const QString& userName) {
    if (GLOB_UserAccount == userAccount) {
        userLogined = true;
        emit this->INTERFACE_LoginAccepted(userAccount,userName);
    }
    emit this->INTERFACE_dealUserLogined(userName);

}

void Client::dealAccountAlreadyLogined(const qint64 userAccount) {
    if(GLOB_UserAccount == userAccount)
        emit INTERFACE_AccountAlreadyLogined(userAccount);
}

void Client::dealNoticeRepeatedNameRejected(const qint64 userAccount) {
    emit INTERFACE_repeatedName();
}


void Client::dealNoticeNameModified(const qint64 userAccount,const QString& userName,const QString& newName) {
    if (userAccount == GLOB_UserAccount)
        emit INTERFACE_NameModifyAccepted(userAccount,newName);
    else
        emit this->INTERFACE_NoticeUserNameModified(userAccount,userName,newName);
}

void Client::dealNoticeAccountOccupied(const qint64 userAccount) {
    if(this->userLogined)
        return;

    emit this->INTERFACE_NoticeAccountOccupied(userAccount);
}

void Client::dealNoticeRegisterAccepted(const qint64 userAccount,const QString& userName,const QString& extName) {
    emit this->INTERFACE_NoticeRegisterAccepted(userAccount,userName,extName);
}

void Client::INTERFACE_registerRequest(const qint64 userAccount,const QString& userPassword,const QString& userName) {
    emit this->RegisterRequest(userAccount,userPassword,userName);
}

void Client::INTERFACE_LoginRequest(const qint64 userAccount,const QString& userPassword) {
    qDebug() << "发送登录请求";
    emit this->SendLoginRequest(userAccount,userPassword);

}

void Client::INTERFACE_SendUserMessage(const QString &senderName, const QString &receiverName, const QString &message) {

    emit this->SendUserMessage(senderName,receiverName,message);

}

void Client::INTERFACE_retryConnect(const qint64 userAccount,const QString& userPassword) {


    this->deleteChatThread();

    this->chat_thread  = new QThread;
    this->client_network_manager = new ClientNetworkManager;
    this->client_network_manager->moveToThread(this->chat_thread);

    //线程启动
    connect(this->chat_thread,&QThread::started,this->client_network_manager,&ClientNetworkManager::initializeSocket,Qt::DirectConnection);
    this->chat_thread->start();


    //重新连接
    this->connect_Init();

    this->INTERFACE_LoginRequest(userAccount,userPassword);


}


void Client::INTERFACE_UserNameModifyRequest(const qint64 userAccount, const QString &newName) {
    emit this->NameModifyRequest(userAccount,newName);
}


