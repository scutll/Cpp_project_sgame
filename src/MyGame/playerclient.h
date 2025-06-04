#ifndef PLAYERCLIENT_H
#define PLAYERCLIENT_H

#include <QString>
#include "playernetwork.h"

#include <QObject>

class PlayerClient : public QObject
{
    Q_OBJECT
public:
    PlayerClient(const qint64 userAccount,QObject *parent = nullptr);
    // PlayerClient(QWidget *parent=nullptr);
    ~PlayerClient();


private:
    QThread* game_thread = nullptr;
    PlayerNetwork* player_network_manager = nullptr;
    bool playerLogined = false;
    bool battling = false;
    qint64 playerAccount = 0;
    qint64 otherPlayer = 0;

public:
    void deletePlayerThread();
    void connect_Init();    //构建GN和Client之间的沟通
    void dealStartGame(const qint64 playerAccount,const qint64 oth_player);
    void dealLoginAccepted(const qint64 playerAccount);
    void dealINTERFACE_userQuited(const qint64 playerAccount);
    void dealINTERFACE_userFinished(const qint64 playerAccount);

    void dealWinForQuit(const qint64 playerAccount);
    void dealWinGame(const qint64 winnerAccount);
    void dealLoseGame(const qint64 loserAccount);

signals:
    void SendLoginRequest(const qint64 playerAccount);
    void playerQuited(const qint64 playerAccount);
    void playerFinished(const qint64 playerAccount);




    //外部接口
public:
    void INTERFACE_userQuited(const qint64 playerAccount);
    void INTERFACE_userFinished(const qint64 winnerAccount);
    void INTERFACE_SendLoginRequest(const qint64 playerAccount);

signals:
    void INTERFACE_LoginAccepted(const qint64 playerAccount);
    void INTERFACE_StartGame(const qint64 playerAccount,const qint64 oth_player);
    void INTERFACE_WinGame(const qint64 winnerAccount);
    void INTERFACE_WinForQuit(const qint64 playerAccount);
    void INTERFACE_LoseGame(const qint64 loserAccount);


};

#endif // PLAYERCLIENT_H
