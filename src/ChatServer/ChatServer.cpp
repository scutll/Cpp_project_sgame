//
// Created by mxl_scut on 25-5-15.
//
//VERSION 1.0 服务器不做本地记录，玩家直接设置账号和用户名，直接指定消息发送给谁或者全体（All）
#include "ChatServer.h"

ChatServer::ChatServer(QObject *parent)
    :QObject(parent){
    this->tcpServer = new TcpServer(this);
    connect(tcpServer, &TcpServer::newClientConnected, this,&ChatServer::newClientConnection,Qt::DirectConnection);

}

ChatServer::~ChatServer() {
    foreach(QThread* thread,this->clients.keys()) {
        this->clients.value(thread)->deleteLater();
        thread->quit();
        thread->wait();
        thread->deleteLater();
    }
    this->clients.clear();
}


void ChatServer::newClientConnection(const qintptr handle) {
    QThread* thread = new QThread;
    //请求连接的客户端的handle
    ClientWork* work = new ClientWork(handle);
    work->moveToThread(thread);
    this->clients.insert(thread,work);

    connect(thread,&QThread::started,work,&ClientWork::initializeSocket,Qt::DirectConnection);
    thread->start();

    //用户断联的处理
    connect(work,&ClientWork::NoticeClientDisconnected,this,&ChatServer::dealNoticeClientDisconnected,Qt::QueuedConnection);
    connect(this,&ChatServer::transferNoticeDisconnected,work,&ClientWork::dealNoticeClientDisconnected,Qt::QueuedConnection);
    connect(work,&ClientWork::updateLocalUserData,this,&ChatServer::dealUpdateLocalUserData,Qt::QueuedConnection);

    //通知
    connect(work,&ClientWork::clientDisconnected,this,&ChatServer::dealClientDisconnected,Qt::QueuedConnection);

    //用户登录
    connect(work,&ClientWork::newClientLogin,this,&ChatServer::dealNewClientLogin,Qt::QueuedConnection);
    connect(this,&ChatServer::transferNewClientLogin,work,&ClientWork::noticeClientLogin,Qt::QueuedConnection);

    //接收普通消息
    connect(work,&ClientWork::acceptUserNormalMessage,this,&ChatServer::dealAcceptUserNormalMessage,Qt::QueuedConnection);
    connect(this,&ChatServer::transferAcceptUserNormalMessage,work,&ClientWork::sendUserMessageToReceiver,Qt::QueuedConnection);
}

void ChatServer::dealNoticeClientDisconnected(const QString &userAccount) {
    emit this->transferNoticeDisconnected(userAccount);
}


void ChatServer::dealUpdateLocalUserData(const QString &userAccount, const QString &type) {
    if (type == "1") {
        qDebug() << userAccount << "added to users:";
        existingUsers.push_back(userAccount);
    }
    else if (type == "0") {
        qDebug() << userAccount << "removed from users:";
        for (int i=0;i < existingUsers.size();i++) {
            if (existingUsers.at(i) == userAccount) {
                existingUsers.erase(existingUsers.begin()+i);
                break;
            }
        }
    }

    qDebug() << "existingUsers: ";
    for (const QString& user: this->existingUsers) {
        qDebug() << user;
    }
}



void ChatServer::dealClientDisconnected(const int socket_id) {
    for (QThread* thread : this->clients.keys()) {
        if (this->clients.value(thread)->SocketId() == socket_id) {
            this->clients.value(thread)->deleteLater();

            thread->requestInterruption();
            thread->exit();
            thread->wait();
            this->clients.remove(thread);
            thread->deleteLater();
            break;
        }
    }

}



void ChatServer::dealNewClientLogin(const QString &usrName) {
    qDebug() << usrName << "loginned ";
    emit transferNewClientLogin(usrName);
    dealUpdateLocalUserData(usrName,"1");
}



void ChatServer::dealAcceptUserNormalMessage(const QString &senderName, const QString &receiverName,
    const QString &message) {
    emit transferAcceptUserNormalMessage(senderName,receiverName,message);

}
