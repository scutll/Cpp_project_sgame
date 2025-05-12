#include "server.h"


ClientSocket::ClientSocket(QObject* parent)
    : QTcpSocket(parent)
{
    nextBlockSize = 0;

    connect(this,SIGNAL(readyRead()),this,SLOT(recv_msg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(deleteLater()));
}

//客户端发送协议 quint16(长度) + quint16(请求类型，尽管现在只有一种字符串，暂时默认为0) + msg
void ClientSocket::recv_msg(){
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_6_8);

    forever{

        if(nextBlockSize == 0){
            if(bytesAvailable() < sizeof(quint16))
                return;

            in >> nextBlockSize;
        }

        if(bytesAvailable() < nextBlockSize)
            return;
    }

    quint16 cmd;
    QString msg;

    in >> cmd;
    if(cmd == 0){
        in >> msg;
    }

    qDebug() << msg;

    // QDataStream out(this);
    // out << quint16(0xffff); //关闭连接
}
