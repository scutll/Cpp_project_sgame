//
// Created by mxl_scut on 25-5-15.
//

#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QTcpServer>
#include <QObject>


class TcpServer :public QTcpServer{
    Q_OBJECT

public:
    TcpServer(QObject *parent);
    ~TcpServer();
protected:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;
signals:
    void newClientConnected(const qintptr handle);


};



#endif //TCPSERVER_H
