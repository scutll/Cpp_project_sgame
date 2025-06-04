#include "playernetwork.h"

#include <QThread>
#include "global.h"

PlayerNetwork::PlayerNetwork(QObject *parent)
    : QObject{parent}
{

}

PlayerNetwork::~PlayerNetwork() {
    qDebug() << "游戏客户端线程销毁 " << QThread::currentThreadId();
}

void PlayerNetwork::initializeSocket() {
    qDebug() << "initializing socket , thread id: " << QThread::currentThreadId();
    this->socket = new QTcpSocket(this);
    this->socket->setSocketOption(QAbstractSocket::KeepAliveOption,true);
    this->socket->connectToHost(QHostAddress("127.0.0.1"),quint16(520134));

    if (!this->socket->waitForConnected()) {
        emit this->connectErrorSignal(this->socket->errorString());
        qDebug() << this->socket->errorString();
        return;
    }

    GLOB_IsConnectedToGameServer = true;

    //服务器
    //服务器事件通知到主线程
    connect(this->socket,&QTcpSocket::readyRead, this, &PlayerNetwork::ReadData,Qt::DirectConnection);
    connect(this->socket,&QTcpSocket::disconnected,this,&PlayerNetwork::dealServerDisconnected,Qt::DirectConnection);
    emit this->connectedSignal();


}

void PlayerNetwork::dealServerDisconnected(){
    qDebug() << "服务器断开连接";
}


void PlayerNetwork::dealSendLoginRequest(const qint64 playerAccount) {
    this->playerAccount = playerAccount;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    qDebug() << "game connect request: " << playerAccount;
    out << quint16(0);
    out << qint16(MSGTYPE::LoginRequest);

    out << playerAccount;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    this->socket->write(block);
}


void PlayerNetwork::dealPlayerFinished(const qint64 playerAccount) {
    this->playerAccount = playerAccount;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    qDebug() << "player Finished : " << playerAccount;
    out << quint16(0);
    out << qint16(MSGTYPE::PlayerFinished);

    out << playerAccount;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    this->socket->write(block);
}

void PlayerNetwork::dealPlayerQuited(const qint64 playerAccount) {
    this->playerAccount = playerAccount;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    qDebug() << "game connect request: " << playerAccount;
    out << quint16(0);
    out << qint16(MSGTYPE::PlayerQuited);

    out << playerAccount;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    this->socket->write(block);
}


void PlayerNetwork::ReadData() {
    QDataStream in(this->socket);
    in.setVersion(QDataStream::Qt_6_8);
    qDebug() << "message received";

    forever{
        if (nextBlockSize == 0) {
            if (socket->bytesAvailable() < sizeof(quint16))
                break;
            in >> nextBlockSize;
        }

        if (socket->bytesAvailable() == 0xffff) {
            nextBlockSize = 0;
            break;
        }
        if (socket->bytesAvailable() < nextBlockSize)
            break;

        in >> MSG_TYPE;

        if (MSG_TYPE == MSGTYPE::LoginAccepted) {
            qint64 account;
            in >> account;
            if (account != this->playerAccount)
                return;

            qDebug() << "登录成功 " << playerAccount;
            emit this->LoginAcceptedSignal(account);
        }

        else if (MSG_TYPE == MSGTYPE::MatchSuccess) {
            qint64 player, otherPlayer;
            in >> player >> otherPlayer;
            if (player != this->playerAccount)
                return;

            this->otherPlayer = otherPlayer;
            emit this->StartGame(player, otherPlayer);
            qDebug() << "游戏开始: " << player << " vs " << otherPlayer;

        }

        else if (MSG_TYPE == MSGTYPE::WinWinWin) {
            qint64 winnerAccount;
            in >> winnerAccount;
            if (this->playerAccount != winnerAccount)
                return;

            qDebug() << "用户胜利";
            emit noticeWinGame((winnerAccount));
        }

        else if (MSG_TYPE == MSGTYPE::WinForQuit) {
            qint64 playerAccount;
            in >> playerAccount;
            if (this->playerAccount != playerAccount)
                return;

            qDebug() << "对方退出";
            emit noticeWinForQuit(playerAccount);
        }

        else if (MSG_TYPE == MSGTYPE::LoseGame) {
            qint64 loserAccount;
            in >> loserAccount;
            if (this->playerAccount != loserAccount)
                return;

            qDebug() << "对方完成，失败";

            emit noticeLoseGame(loserAccount);

        }

        else if (MSG_TYPE == MSGTYPE::WaitingForMatch) {
            qint64 player;
            in >> player;
            if (player == this->playerAccount) {
                qDebug() << "还需等待匹配";
                emit noticeWaitingForMatching(player);
            }
        }



    }


}
