#ifndef GAMESERVER_H
#define GAMESERVER_H

#include "gameclientwork.h"
#include "tcpserver.h"
#include <QObject>
#include <QMap>
#include <QString>
#include <QQueue>
#include <QPair>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include <QVector>

class GameServer : public QObject
{
    Q_OBJECT
public:
    GameServer(QObject* parent = nullptr);
    ~GameServer();
    void newClientConnection(qintptr handle);

private:
    void dealPlayerLogined(const qint64 playerAccount);
    void dealJoinMatching(const qint64 playerAccount);
    void dealOneQuited(const qint64 playerAccount);
    void dealOneFinished(const qint64 playerAccount);

    void removePlayingPairs(const qint64 playerAccount);
    qint64 FindOtherBattler(const qint64 playerAccount);

private:
    QMap<qint64,GameClientWork*> players;
    QQueue<qint64> matching_queue;
    QMutex mutex;
    QVector<QPair<qint64,qint64>> playing_pairs;
    TcpServer *tcpserver;



signals:
    void transferPlayerLogined(const qint64 playerAccount);
    void transferWaitingForMatching(const qint64 playerAccount);
    void transferMatchSucess(const qint64 playerAccount,const qint64 oth_player, const package& Game);
    void PlayerSucceed_Quit(const qint64 winnerAccount);
    void PlayerSucceed_Win(const qint64 winnerAccount);
    void PlayerLose(const qint64 winnerAccount);

};

#endif // GAMESERVER_H
