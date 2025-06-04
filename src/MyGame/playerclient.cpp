#include "playerclient.h"
#include <QThread>

#include "global.h"

PlayerClient::PlayerClient(const qint64 userAccount, QObject *parent)
    : QObject{parent} {
    this->playerAccount = userAccount;
    this->game_thread = new QThread;
    this->player_network_manager = new PlayerNetwork;
    this->player_network_manager->moveToThread(game_thread);

    //线程启动
    connect(this->game_thread,&QThread::started,this->player_network_manager,&PlayerNetwork::initializeSocket);
    this->game_thread->start();


    this->connect_Init();
}


PlayerClient::~PlayerClient() {
    this->deletePlayerThread();
}

void PlayerClient::deletePlayerThread(){

    qDebug() << "删除游戏线程中:";
    if (this->game_thread != Q_NULLPTR) {
        this->player_network_manager->deleteLater();
        this->player_network_manager= Q_NULLPTR;

        this->game_thread->exit();
        this->game_thread->wait();
        this->game_thread->deleteLater();
        this->game_thread = Q_NULLPTR;

    }

}



void PlayerClient::connect_Init() {

    //连接失败

    //用户登录
    connect(this,&PlayerClient::SendLoginRequest,this->player_network_manager,&PlayerNetwork::dealSendLoginRequest,Qt::QueuedConnection);
    connect(this->player_network_manager,&PlayerNetwork::LoginAcceptedSignal,this,&PlayerClient::dealLoginAccepted,Qt::QueuedConnection);

    //匹配开始游戏
    connect(this->player_network_manager,&PlayerNetwork::StartGame,this,&PlayerClient::dealStartGame,Qt::QueuedConnection);
    connect(this,&PlayerClient::playerQuited,this->player_network_manager,&PlayerNetwork::dealPlayerQuited,Qt::QueuedConnection);
    connect(this,&PlayerClient::playerFinished,this->player_network_manager,&PlayerNetwork::dealPlayerFinished,Qt::QueuedConnection);

    connect(this->player_network_manager,&PlayerNetwork::noticeWinForQuit,this,&PlayerClient::dealWinForQuit,Qt::QueuedConnection);
    connect(this->player_network_manager,&PlayerNetwork::noticeWinGame,this,&PlayerClient::dealWinGame,Qt::QueuedConnection);
    connect(this->player_network_manager,&PlayerNetwork::noticeLoseGame,this,&PlayerClient::dealLoseGame,Qt::QueuedConnection);


}


void PlayerClient::dealLoginAccepted(const qint64 playerAccount) {
    if (GLOB_UserAccount != playerAccount)
        return;
    qDebug() << playerAccount << "登录成功";
    emit this->INTERFACE_LoginAccepted(playerAccount);
}


void PlayerClient::dealStartGame(const qint64 playerAccount, const qint64 oth_player) {

    if (battling) {
        qDebug() << "battling with: " << this->otherPlayer;
        return;
    }

    emit this->INTERFACE_StartGame(playerAccount,oth_player);
    this->battling = true;
    this->otherPlayer = oth_player;
}


void PlayerClient::dealINTERFACE_userFinished(const qint64 playerAccount) {
    emit this->playerFinished(playerAccount);
    this->battling = false;
}

void PlayerClient::dealINTERFACE_userQuited(const qint64 playerAccount) {
    emit this->playerQuited(playerAccount);
    this->battling = false;
}


void PlayerClient::dealWinForQuit(const qint64 playerAccount) {
    emit INTERFACE_WinForQuit(playerAccount);
}

void PlayerClient::dealWinGame(const qint64 winnerAccount) {
    emit INTERFACE_WinGame(winnerAccount);
}

void PlayerClient::dealLoseGame(const qint64 loserAccount) {
    emit INTERFACE_LoseGame(loserAccount);
}

void PlayerClient::INTERFACE_userQuited(const qint64 playerAccount) {
    dealINTERFACE_userQuited(playerAccount);
}

void PlayerClient::INTERFACE_userFinished(const qint64 winnerAccount) {
    dealINTERFACE_userFinished(winnerAccount);
}

void PlayerClient::INTERFACE_SendLoginRequest(const qint64 playerAccount) {
    emit this->SendLoginRequest(playerAccount);
}



