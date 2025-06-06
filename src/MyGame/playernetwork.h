#ifndef PLAYERNETWORK_H
#define PLAYERNETWORK_H

#include <QMutex>
#include <QObject>
#include <QTcpSocket>
#include "../../src/TerminalVersion/scene.h"
#include "../../src/TerminalVersion/save_package.h"

class PlayerNetwork : public QObject
{
    Q_OBJECT
    enum MSGTYPE{
        /*server接收*/
        LoginRequest = 0, MatchRequest, PlayerFinished, PlayerQuited,

        /*client接收*/
        LoginAccepted, WinWinWin, WinForQuit, LoseGame, WaitingForMatch, MatchSuccess,

    };
public:
    PlayerNetwork(QObject *parent = nullptr);
    ~PlayerNetwork();
    void initializeSocket();
    void ReadData();


private:
    quint16 nextBlockSize = 0;
    qint16 MSG_TYPE = -1;
    QTcpSocket *socket = nullptr;
    QMutex mutex;
    qint64 playerAccount = 0;
    qint64 otherPlayer = 0;


public:
    void dealSendLoginRequest(const qint64 playerAccount);
    void dealPlayerQuited(const qint64 playerAccount);
    void dealPlayerFinished(const qint64 playerAccount);
    void dealServerDisconnected();
    void dealSendMatchRequest(const qint64 playerAccount);


signals:
    void noticeServerDisconnecred();
    void LoginAcceptedSignal(const qint64 playerAccount);
    void joinMatching(const qint64 playerAccount);
    void StartGame(const qint64 playerAccount, const qint64 oth_player, const package& GamePkg);
    void noticeWaitingForMatching(const qint64 playerAccount);
    void noticeWinForQuit(const qint64 winnerAccount);
    void noticeWinGame(const qint64 winnerAccount);
    void noticeLoseGame(const qint64 loserAccount);
    void connectErrorSignal(const QString error);
    void connectedSignal();

};

#endif // PLAYERNETWORK_H
