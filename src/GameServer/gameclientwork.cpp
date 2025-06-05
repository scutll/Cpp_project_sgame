#include "gameclientwork.h"
QByteArray serialize_Package(const package& pkg);
GameClientWork::GameClientWork(const quintptr &handle, QObject *parent):QObject(parent)
{
    this->socketDescripter = handle;
}

GameClientWork::~GameClientWork() {
    qDebug() << __TIME__ << " 客户端销毁: "<<QThread::currentThreadId();

}

void GameClientWork::initializeSocket(){
    qDebug() << __TIME__ << " 客户端线程启动: " << QThread::currentThreadId();
    this->socket_id = QString::number(qintptr(QThread::currentThreadId())).toInt();

    qDebug() << "socket id: " << this->socket_id;
    this->socket = new QTcpSocket(this);
    this->socket->setSocketDescriptor(this->socketDescripter);
    qDebug() << this->socket->peerAddress() << this->socket->peerPort();

    //接收消息
    connect(this->socket,&QTcpSocket::readyRead,this,&GameClientWork::ReadData,Qt::QueuedConnection);

    //用户断联
    connect(this->socket,&QTcpSocket::disconnected,this,&GameClientWork::dealClientDisconnected,Qt::QueuedConnection);


}

void GameClientWork::dealClientDisconnected(){
    if(this->playerAccount) {
        emit this->noticeOneQuited(this->playerAccount);
        qDebug() << this->playerAccount << "离线";
    }
}


void GameClientWork::noticePlayerLogined(const qint64 playerAccount){
    if(this->playerAccount != playerAccount)
        return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_8);
    out << quint16(0);
    out << qint16(MSGTYPE::LoginAccepted);
    out << playerAccount;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    qDebug() << playerAccount << " 登录成功";
    this->socket->write(block);
}

void GameClientWork::dealtransferMatchSuccess(const qint64 playerAccount, const qint64 oth_player, const package& Game){
    if(this->playerAccount != playerAccount && this->playerAccount != oth_player)
        return;
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_8);

    out << quint16(0);
    out << qint16(MSGTYPE::MatchSuccess);
    if(this->playerAccount == playerAccount){
        qDebug() << "匹配成功 " << playerAccount << " vs " << oth_player;
        out << playerAccount << oth_player;
    }
    else if(this->playerAccount == oth_player){
        qDebug() << "匹配成功 " << oth_player << " vs " << playerAccount;
        out << oth_player << playerAccount;
    }

    block += serialize_Package(Game);

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));


    this->socket->write(block);
}

void GameClientWork::dealtransferWaitingForMatching(const qint64 playerAccount){
    if(this->playerAccount != playerAccount)
        return;
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_8);

    out << quint16(0);
    out << qint16(MSGTYPE::WaitingForMatch);
    out << playerAccount;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    qDebug() << playerAccount << "正在等待匹配";
    this->socket->write(block);

}

void GameClientWork::SendWinMessage_Quit(const qint64 winnerAccount){
    if(this->playerAccount != winnerAccount)
        return;
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_8);

    out << quint16(0);
    out << qint16(MSGTYPE::WinForQuit);
    out << winnerAccount;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    qDebug() << "发送信息: " << playerAccount << " 获胜";
    this->socket->write(block);

}

void GameClientWork::SendWinMessage(const qint64 winnerAccount){
    if(this->playerAccount != winnerAccount)
        return;
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_8);

    out << quint16(0);
    out << qint16(MSGTYPE::WinWinWin);
    out << winnerAccount;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    qDebug() << "发送信息: " << winnerAccount << " 获胜";
    this->socket->write(block);
}

void GameClientWork::SendLoseMessage(const qint64 loserAccount){
    if(this->playerAccount != loserAccount)
        return;
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_8);

    out << quint16(0);
    out << qint16(MSGTYPE::LoseGame);
    out << loserAccount;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    qDebug() << "发送信息: " << loserAccount << " 失败";
    this->socket->write(block);
}


void GameClientWork::ReadData(){
    qDebug() << __TIME__ << "message received";

    QDataStream in(this->socket);
    in.setVersion(QDataStream::Qt_6_8);

    forever{
        if(nextBlockSize == 0){
            if(this->socket->bytesAvailable() < sizeof(quint16))
                break;
            in >> nextBlockSize;
        }
        if(nextBlockSize == 0xffff){
            nextBlockSize = 0;
            break;
        }

        if(this->socket->bytesAvailable() < nextBlockSize){
            break;
        }

        in >> this->MSG_TYPE;
        qDebug() << MSG_TYPE;

        if(MSG_TYPE == MSGTYPE::LoginRequest){
            qint64 playerAccount;
            in >> playerAccount;
            qDebug() << "收到登录请求: " << playerAccount;
            if(battling)
                return;
            this->playerAccount = playerAccount;
            qDebug() << playerAccount << " 登录中";
            emit playerLogined(this->playerAccount);
        }
        else if(MSG_TYPE == MSGTYPE::MatchRequest){
            qint64 playerAccount;
            in >> playerAccount;
            qDebug() << "收到匹配请求: " << playerAccount;
            if(this->playerAccount && this->playerAccount == playerAccount && !this->battling){
                qDebug() << "向服务器发送匹配请求";
                emit joinMatching(playerAccount);
            }
        }
        else if(MSG_TYPE == MSGTYPE::PlayerQuited){
            
            qint64 quiterAccount;
            in >> quiterAccount;
            qDebug() << "用户退出: " << quiterAccount;
            emit this->noticeOneQuited(quiterAccount);
        }
        else if(MSG_TYPE == MSGTYPE::PlayerFinished){
            qint64 winnerAccount;
            in >> winnerAccount;

            if(winnerAccount != this->playerAccount)
                return;
            qDebug() << "用户完成: " << winnerAccount;
            emit this->noticeOneFinished(winnerAccount);
        }

        nextBlockSize = 0;
        MSG_TYPE = -1;
    }

}


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
