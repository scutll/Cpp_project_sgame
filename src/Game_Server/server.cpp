#include "server.h"
#include <QString>
#include "../../src/save_package.h"
#include "Game_handler.h"

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




Server::Server(){
    serverIP = QString("127.0.0.1");
    serverPort = 125521;

    server = new QTcpServer(this);
    connect(server,&QTcpServer::newConnection,this,&Server::handle_new_connection);
    //开始监听
    if (!server->listen(QHostAddress::Any, serverPort)) {
        qDebug() << "Server failed to start:" << server->errorString();
        exit(1);
    }
    else
        qDebug() << "Server started on port " << serverPort;
}

void Server::handle_new_connection(){
    while(this->server->hasPendingConnections()){
        QTcpSocket* new_player = server->nextPendingConnection();
        //这个new_player 是服务器用来与客户端沟通的一个对象，服务端通过new_player收到客户端的信息会发出readyRead的信号
        qDebug() << "New player connected:" << new_player->peerAddress().toString();
        connect(new_player,&QTcpSocket::readyRead,this,&Server::handle_match_request);
        connect(new_player,&QTcpSocket::readyRead,this,&Server::recv_msg);
        // matching_players_list.push(new_player);
    }
}

void Server::handle_match_request(){
    QTcpSocket* player = qobject_cast<QTcpSocket*>(sender());   //获取发送信号的对象
    if(!player) return;

    QByteArray buffer = player->readAll();
    QDataStream in(buffer);
    QString msg;
    in >> msg;
    qDebug() << msg;
    // QString msg = QString::fromUtf8(buffer);

    if(msg == QString("match_request")){
        QMutexLocker locker(&queuemutex);
        matching_players_list.push(player);
        qDebug()<<"新用户加入队列";
        send_msg(player,"积极寻找对手中，当前匹配队列长度：" + QString::number(matching_players_list.size()));


        qDebug()<<"队列长度: "<<this->matching_players_list.size();
        locker.unlock();
    }
}


void Server::match_two(QTcpSocket* lhs,QTcpSocket* rhs){
    std::pair pair_(lhs,rhs);
    //添加进对战的队列
    this->compating_players_list.push_back(pair_);

    Scene game;
    game.generate();


    send_msg(lhs,game.package_());

    QThread* thread = new QThread;
    BattleHandler* handler= new BattleHandler(lhs,rhs);

    handler->moveToThread(thread);
    connect(thread,&QThread::started,handler,&BattleHandler::startBattle);
    connect(handler, &BattleHandler::finished, thread, &QThread::quit);
    connect(handler, &BattleHandler::finished, handler, &BattleHandler::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}




//运行线程
class MatchThread : public QThread {
public:
    explicit MatchThread(Server* server, QObject* parent = nullptr) : QThread(parent), server(server) {}

protected:
    void run() override {
        while (true) {
            QMutexLocker locker(&server->queuemutex);
            if (server->matching_players_list.size() >= 2) {

                qDebug()<<"match one pair";
                QTcpSocket* player1 = server->matching_players_list.front();
                server->matching_players_list.pop();
                QTcpSocket* player2 = server->matching_players_list.front();
                server->matching_players_list.pop();
                locker.unlock(); // 释放锁，避免在匹配过程中阻塞其他线程

                server->match_two(player1, player2);
            }
        }
    }

private:
    Server* server;
};

void Server::run() {
    qDebug()<<"server Running:";
    MatchThread* matchThread = new MatchThread(this);
    matchThread->start();
}


// void Server::run(){
//     while(true){
//         QMutexLocker locker(&queuemutex);
//         if(this->matching_players_list.size() >= 2){
//             QTcpSocket* player1 = matching_players_list.front();
//             matching_players_list.pop();
//             QTcpSocket* player2 = matching_players_list.front();
//             matching_players_list.pop();

//             locker.unlock();

//             match_two(player1,player2);

//         }
//     }
// }




//消息协议格式 quint16(长度) + quint16(类型) + QString/package

void Server::recv_msg(){
    qDebug() << "msg received";
    QTcpSocket* player = qobject_cast<QTcpSocket*>(sender());
    if(!player)
        return;
    QDataStream block(player);
    block.setVersion(QDataStream::Qt_6_8);
    //

}



void Server::send_msg(QTcpSocket* socket,const QString msg){
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_8);
    stream << quint16(0);
    qDebug()<<"sending: "<<msg;
    stream << quint16(0);
    // stream << length;
    stream << msg;
    stream.device()->seek(0);
    stream << quint16(bytes.size() - sizeof(quint16));

    socket->write(bytes);
}
void Server::send_msg(QTcpSocket* socket,const package pkg){
    QByteArray buffer;
    QDataStream data(&buffer, QIODevice::WriteOnly);
    data.setVersion(QDataStream::Qt_6_8);

    data << quint16(0);
    data << quint16(1);
    buffer += serialize_Package(pkg);

    data.device()->seek(0);
    data << quint16(buffer.size() - sizeof(quint16));

    socket->write(buffer);
}

void Server::send_package_randomgame(QTcpSocket* player){
    Scene game;
    game.generate();

    send_msg(player,game.package_());
}

Server::~Server() {}
