//
// Created by mxl_scut on 25-5-15.
//

#include "../include/ClientNetworkManager.h"

#include "../include/Client.h"

void ClientNetworkManager::initializeSocket() {
    qDebug() << "Initializing socket , threadid: " << QThread::currentThreadId();
    this->socket = new QTcpSocket(this);
    this->socket->setSocketOption(QAbstractSocket::KeepAliveOption,true);
    this->socket->connectToHost(QHostAddress("127.0.0.1"), quint16(125521));

    if (!this->socket->waitForConnected()) {
        emit this->connectErrorSignal();
        qDebug() << this->socket->errorString();
        return;
    }

    GLOB_IsConnectedToServer = true;
    connect(this->socket,&QTcpSocket::readyRead, this, &ClientNetworkManager::ReadData,Qt::DirectConnection);
    emit this->connectedSignal();
    this->sendLoginInfo();




}
