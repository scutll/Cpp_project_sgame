#include "player_connect.h"
#include "../../src/scene.h"
#include "../../src/save_package.h"
#include "player_connect.h"
#include <QByteArray>
#include <QDataStream>
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

    if(Server_socket)
        delete Server_socket;

    Server_socket = new QTcpSocket;
    Server_socket->abort();


    Server_socket->connectToHost(this->ServerIP,this->ServerPort);
    bool bRet = Server_socket->waitForConnected(1000);

    if(bRet){
        qDebug()<<"connection succeed!";

        this->connect(Server_socket,SIGNAL(readyRead()),this,SLOT(recv_msg()));
    }
    else{
        qDebug()<<"failed to connect to the server!";
    }

    return bRet;
}

//发送数据
void PlayerConnector::send_msg(const QString msg){
    QByteArray buffer = msg.toLocal8Bit();

    Server_socket->write(buffer);
}

void PlayerConnector::send_match_request(){
    send_msg(QString("match request"));
}

void PlayerConnector::send_complete_msg(){
    send_msg(QString("completed"));
}




//接收数据
//字节流的第一位标识是0：QString还是1：package
void PlayerConnector::recv_msg(){
    QByteArray buffer = Server_socket->readAll();
    QDataStream stream(buffer);

    int sign;
    stream >> sign; //读取标识位

    if(sign == 0){
        stream >> this->new_msg;
        //设置一个信号来处理信息
    }
    else if(sign == 1){
        this->new_pkg = deserialization_Package(stream);
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
