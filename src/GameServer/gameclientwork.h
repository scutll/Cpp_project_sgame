#ifndef GAMECLIENTWORK_H
#define GAMECLIENTWORK_H

#include <QObject>
#include <QString>
#include <QQueue>
#include <QPair>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include <QVector>
#include "../../src/TerminalVersion/scene.h"
#include "../../src/TerminalVersion/save_package.h"

class GameClientWork : public QObject
{
    Q_OBJECT

    enum MSGTYPE{
        /*server接收*/
        LoginRequest = 0, MatchRequest, PlayerFinished, PlayerQuited,

        /*client接收*/
        LoginAccepted, WinWinWin, WinForQuit, LoseGame, WaitingForMatch, MatchSuccess,

    };

public:
    GameClientWork(const quintptr& handle,QObject* parent = nullptr);
    ~GameClientWork();
public:
    void initializeSocket();
    int socketId(){return socket_id;}
    void noticePlayerLogined(const qint64 playerAccount);
    void dealtransferWaitingForMatching(const qint64 playerAccount);
    void dealtransferMatchSuccess(const qint64 playerAccount, const qint64 oth_player, const package& Game);
    void SendWinMessage_Quit(const qint64 winnerAccount);
    void SendWinMessage(const qint64 winnerAccount);
    void SendLoseMessage(const qint64 loserAccount);

private:
    void dealClientDisconnected();
    void ReadData();


private:
    QTcpSocket* socket = nullptr;
    qintptr socketDescripter;
    qint16 MSG_TYPE = -1;
    qint64 playerAccount = 0;
    quint16 nextBlockSize = 0;
    int socket_id = 0;
    bool battling = false;


signals:
    void playerLogined(const qint64 playerAccount);
    void joinMatching(const qint64 playerAccount);
    void noticeOneQuited(const qint64 playerAccount);
    void noticeOneFinished(const qint64 playerAccount);


};

#endif // GAMECLIENTWORK_H
