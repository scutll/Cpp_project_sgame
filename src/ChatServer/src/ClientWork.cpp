//
// Created by mxl_scut on 25-5-15.
//

#include "../include/ClientWork.h"

ClientWork::ClientWork(const quintptr &handle, QObject *parent) :QObject(parent){
    this->socketDescriptor = handle; //获取套接字描述符
}

ClientWork::~ClientWork() {
    qDebug() << __TIME__ << " 客户端销毁: "<<QThread::currentThreadId();

}

void ClientWork::initalizeSocket() {
    qDebug() << __TIME__ << " 客户端线程启动：" << QThread::currentThreadId();
    this->socket_id = QString::number(quintptr(QThread::currentThreadId())).toInt();

    qDebug() << "socket id: " << this->socket_id;
    this->socket = new QTcpSocket(this);
    this->socket->setSocketDescriptor(this->socketDescriptor);
    qDebug() << this->socket->peerAddress() << this->socket->peerPort();

    //接收消息
    connect(this->socket,&QTcpSocket::readyRead,this,&ClientWork::ReadData, Qt::DirectConnection);

    //socket断联
    connect(this->socket,&QTcpSocket::disconnected,this,&ClientWork::dealClientDisconnected, Qt::DirectConnection);
}


void ClientWork::dealClientDisconnected() {
    emit NoticeClientDisconnected(this->userAccount);
    emit updateLocalUserData(this->userAccount,"0");
    emit clientDisconnected(socket_id);
}
