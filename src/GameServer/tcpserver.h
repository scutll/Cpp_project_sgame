#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(QObject* parent);
    ~TcpServer();

protected:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

signals:
    void newClientConnected(qintptr handle);
};

#endif // TCPSERVER_H
