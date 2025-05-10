#include "player_connect.h"
#include "../../src/scene.h"
#include "../../src/save_package.h"
#include "player_connect.h"
#include <QByteArray>
#include <QDataStream>
#include <QTimer>
#include <QApplication>
package deserialization_Package(QDataStream& stream);
QByteArray serialize_Package(const package& pkg);


void PlayerConnector::set_IP(const QString IP){
    ServerIP = IP;
}
void PlayerConnector::set_port(const int port){
    ServerPort = port;
}


PlayerConnector::PlayerConnector(){
    ServerIP = QString("127.0.0.1");
    ServerPort = 125521;

}

bool PlayerConnector::connect_to_server(){

    // Server_socket = new QTcpSocket;


    // Server_socket->connectToHost(this->ServerIP,this->ServerPort);
    // bool bRet = Server_socket->waitForConnected(3000);

    // if(bRet){
    //     qDebug()<<"connection succeed!";

    //     this->connect(Server_socket,&QTcpSocket::readyRead,this,&PlayerConnector::recv_msg);
    // }
    // else{
    //     qDebug()<<"failed to connect to the server!";
    // }

    // return bRet;
    Server_socket = new QTcpSocket(this);

    // 连接信号到槽
    connect(Server_socket, &QTcpSocket::connected, this, &PlayerConnector::onConnected);
    connect(Server_socket, &QTcpSocket::errorOccurred, this, &PlayerConnector::onError);
    connect(Server_socket,&QTcpSocket::readyRead,this,&PlayerConnector::recv_msg);

    // 设置连接超时
    QTimer::singleShot(1000, this, &PlayerConnector::onTimeout); // 1秒超时

    Server_socket->connectToHost(this->ServerIP, this->ServerPort);
    return true; // 返回 true 表示连接请求已发出



}

//发送数据
void PlayerConnector::send_msg(const QString msg){
    QByteArray buffer = msg.toLocal8Bit();

    Server_socket->write(buffer);
}

void PlayerConnector::send_match_request(){
    qDebug()<<"match request sent";
    send_msg(QString("match_request"));
}

void PlayerConnector::send_complete_msg(){
    send_msg(QString("game_completed"));
}




//接收数据
//字节流的第一位标识是0：QString还是1：package 第二个int 是数据长度
void PlayerConnector::recv_msg(){
    emit recv_msg_str("msg received");
    qDebug()<<"msg received";

    // Server_socket->abort();
    // Server_socket->connectToHost(this->ServerIP, this->ServerPort);

    QByteArray buffer = Server_socket->readAll();
    QDataStream stream(buffer);

    int sign;
    stream >> sign; //读取标识位
    // int length;
    // stream >> length;
    if(sign == 0){
        stream >> this->new_msg;
        //设置一个信号来处理信息
        emit recv_msg_str(new_msg);
    }

    else if(sign == 1){
        this->new_pkg = deserialization_Package(stream);
        for(int i=0;i<36;i++)
            qDebug()<<this->new_pkg.map[i].value;
        emit recv_msg_pakcage(new_pkg);
        //设置一个信号来更新地图
    }

}



//地图包序列化
QByteArray serialize_Package(const package& pkg){
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);

    for(int i = 0;i < 20;i++){
        stream << pkg.archive_name[i];
    }
    // 序列化 tm 结构体
    stream << pkg.date.tm_sec;
    stream << pkg.date.tm_min;
    stream << pkg.date.tm_hour;
    stream << pkg.date.tm_mday;
    stream << pkg.date.tm_mon;
    stream << pkg.date.tm_year;
    stream << pkg.date.tm_wday;
    stream << pkg.date.tm_yday;
    stream << pkg.date.tm_isdst;

    for(int i=0;i<36;i++){
        stream << pkg.map[i].value;
        stream << pkg.map[i].status;
    }

    return bytes;

}

//地图包反序列化
package deserialization_Package(QDataStream& stream){
    package pkg;

    for(int i = 0;i < 20;i++){
        stream >> pkg.archive_name[i];
    }
    // 序列化 tm 结构体
    stream >> pkg.date.tm_sec;
    stream >> pkg.date.tm_min;
    stream >> pkg.date.tm_hour;
    stream >> pkg.date.tm_mday;
    stream >> pkg.date.tm_mon;
    stream >> pkg.date.tm_year;
    stream >> pkg.date.tm_wday;
    stream >> pkg.date.tm_yday;
    stream >> pkg.date.tm_isdst;

    for(int i=0;i<36;i++){
        stream >> pkg.map[i].value;
        stream >> pkg.map[i].status;
    }

    return pkg;
}
