//
// Created by mxl_scut on 25-5-15.
//

#include "ClientNetworkManager.h"

#include "Client.h"

ClientNetworkManager::ClientNetworkManager(QObject *parent) : QObject(parent){

}

ClientNetworkManager::~ClientNetworkManager() {
    qDebug() << "客户端线程销毁" << QThread::currentThreadId();
}



void ClientNetworkManager::initializeSocket() {
    qDebug() << "Initializing socket , thread id: " << QThread::currentThreadId();
    this->socket = new QTcpSocket(this);
    this->socket->setSocketOption(QAbstractSocket::KeepAliveOption,true);
    this->socket->connectToHost(QHostAddress("127.0.0.1"), quint16(521125));

    if (!this->socket->waitForConnected()) {
        emit this->connectErrorSignal(this->socket->errorString());
        qDebug() << this->socket->errorString();
        return;
    }

    GLOB_IsConnectedToServer = true;
    //服务器事件通知到主线程
    connect(this->socket,&QTcpSocket::readyRead, this, &ClientNetworkManager::ReadData,Qt::DirectConnection);
    connect(this->socket,&QTcpSocket::disconnected,this,&ClientNetworkManager::dealServerDisconnected,Qt::DirectConnection);
    emit this->connectedSignal();
    this->sendLoginInfo();

}

void ClientNetworkManager::ReadData() {
    QDataStream in(this->socket);
    in.setVersion(QDataStream::Qt_5_15);
    qDebug() << __TIME__ << "msg_received";

    forever{
        if (nextBlockSize == 0){
            qDebug() << "next block size is 0";
            if (socket->bytesAvailable() < sizeof(quint16))
                break;
            in >> nextBlockSize;
        }

        if (nextBlockSize == 0xffff){
            qDebug() << "reading finished";
            nextBlockSize = 0;
            break;
        }

        if (socket->bytesAvailable() < nextBlockSize) {
            qDebug() << "uncomplete data";
            break;
        }

        in >> MSG_TYPE;
        qDebug() << MSG_TYPE;

        if (MSG_TYPE == MSGTYPE::NoticeNewLogin) {
            QString username;
            in >> username;
            qDebug() << username << "Logined";
            emit this->UserLogined(username);
        }

        else if (MSG_TYPE == MSGTYPE::WaitAcceptApplication) {
            //暂未开放
        }

        else if (MSG_TYPE == MSGTYPE::AcceptedApplication) {
            //暂未开放
        }

        else if (MSG_TYPE == MSGTYPE::SendNormalMesssage) {
            QString senderName;
            QString message;
            in >> senderName >> message;

            qDebug() << "from: " << senderName << " " << message;
            emit this->acceptNormalMessage(senderName, message);


        }
        else if (MSG_TYPE == MSGTYPE::SendUserDisconnected) {
            QString senderName;
            in >> senderName;
            qDebug() << senderName << "disconnected";
            emit this->userDisconnectedSignal(senderName);
        }

        nextBlockSize = 0;
        MSG_TYPE = -1;
    }

}

void ClientNetworkManager::dealServerDisconnected() {
    emit this->ServerDisconnectedSignal();
}

bool ClientNetworkManager::isConnected() {
    return socket->state() == QTcpSocket::ConnectedState;
}

void ClientNetworkManager::sendLoginInfo() {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    qDebug() << "login info: " << GLOB_UserName;
    out << quint16(0);
    out << qint16(MSGTYPE::LoginRequest);

    out << QString(GLOB_UserName);

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    this->socket->write(block);

}



void ClientNetworkManager::sendUserNormalMessage(const QString &senderName, const QString &receiverName,
    const QString &message) {
    qDebug() << "sending message: " << "to" << receiverName;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << quint16(0);
    out << qint16(MSGTYPE::NormalMessage);
    out << senderName << receiverName << message;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    this->socket->write(block);

}
