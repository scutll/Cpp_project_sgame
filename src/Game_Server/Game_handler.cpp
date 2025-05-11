#include "Game_handler.h"
#include <QTcpSocket>

BattleHandler::BattleHandler(QTcpSocket* player1, QTcpSocket* player2, QObject* parent)
    : QObject(parent), player1(player1), player2(player2) {
    connect(player1, &QTcpSocket::readyRead, this, &BattleHandler::handlePlayer1);
    connect(player2, &QTcpSocket::readyRead, this, &BattleHandler::handlePlayer2);
}

void BattleHandler::startBattle() {
    qDebug() << "Battle started between" << player1->peerAddress().toString() << "and" << player2->peerAddress().toString();
}

void BattleHandler::handlePlayer1() {
    handlePlayer(player1, player2);
}

void BattleHandler::handlePlayer2() {
    handlePlayer(player2, player1);
}


void send_msg(QTcpSocket* socket,const QString msg);
void BattleHandler::handlePlayer(QTcpSocket* player, QTcpSocket* opponent) {
    QByteArray data = player->readAll();
    QString message = QString::fromUtf8(data);

    if (message == "game_completed") {
        send_msg(player,"you win!\n");
        send_msg(opponent,"you lose\n");
        emit finished();
    } else if (message.isEmpty()) {
        send_msg(opponent,"you win!\n");
        send_msg(player,"you lose\n");
        emit finished();
    }
}

void send_msg(QTcpSocket* socket,const QString msg){
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);

    stream << 0;
    stream << msg;

    socket->write(bytes);
}

