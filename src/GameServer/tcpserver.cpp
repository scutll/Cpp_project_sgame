#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) :QTcpServer(parent){
    if (!this->listen(QHostAddress::Any, 520134))
        return;
    qDebug()<<"server started: listening in port: "<<520134;
}

TcpServer::~TcpServer() {
    if (!this->isListening())
        this->close();
}

void TcpServer::incomingConnection(qintptr handle) {
    emit this->newClientConnected(handle);
}
