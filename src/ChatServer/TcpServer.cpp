//
// Created by mxl_scut on 25-5-15.
//

#include "TcpServer.h"

TcpServer::TcpServer(QObject *parent) :QTcpServer(parent){
    if (!this->listen(QHostAddress::Any, 521125))
            return;
    qDebug()<<"server started: listening in port: "<<521125;
}

TcpServer::~TcpServer() {
    if (!this->isListening())
        this->close();
}

void TcpServer::incomingConnection(qintptr handle) {
    emit this->newClientConnected(handle);
}