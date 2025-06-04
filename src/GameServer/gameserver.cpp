#include "gameserver.h"

GameServer::GameServer(QObject* parent)
    :QObject(parent){
    this->tcpserver = new TcpServer(this);
    connect(tcpserver,&TcpServer::newClientConnected,this,&GameServer::newClientConnection,Qt::DirectConnection);

}

GameServer::~GameServer(){

}

void GameServer::newClientConnection(qintptr handle){
    QThread* thread = new QThread;
    GameClientWork* work = new GameClientWork(handle);
    work->moveToThread(thread);

    connect(thread,&QThread::started,work,&GameClientWork::initializeSocket,Qt::DirectConnection);
    thread->start();


    //下面是GameServer和GameClientWork之间的连接

    //用户加入服务器
    connect(work,&GameClientWork::playerLogined,this,&GameServer::dealPlayerLogined,Qt::QueuedConnection);
    connect(this,&GameServer::transferPlayerLogined,work,&GameClientWork::noticePlayerLogined,Qt::QueuedConnection);

    //用户申请开始匹配
    connect(work,&GameClientWork::joinMatching,this,&GameServer::dealJoinMatching,Qt::QueuedConnection);
    connect(this,&GameServer::transferWaitingForMatching,work,&GameClientWork::dealtransferWaitingForMatching,Qt::QueuedConnection);
    connect(this,&GameServer::transferMatchSucess,work,&GameClientWork::dealtransferMatchSuccess,Qt::QueuedConnection);
    connect(work,&GameClientWork::noticeOneFinished,this,&GameServer::dealOneFinished,Qt::QueuedConnection);
    connect(work,&GameClientWork::noticeOneQuited,this,&GameServer::dealOneQuited,Qt::QueuedConnection);
    connect(this,&GameServer::PlayerSucceed_Quit,work,&GameClientWork::SendWinMessage_Quit,Qt::QueuedConnection);
    connect(this,&GameServer::PlayerSucceed_Win,work,&GameClientWork::SendWinMessage,Qt::QueuedConnection);
    connect(this,&GameServer::PlayerLose,work,&GameClientWork::SendLoseMessage,Qt::QueuedConnection);
}

qint64 GameServer::FindOtherBattler(const qint64 playerAccount){
    qDebug() << "正在寻找 " << playerAccount << "的配对玩家: ";
    for(auto& pair_ : this->playing_pairs){
        if(pair_.first != playerAccount && pair_.second != playerAccount)
            continue;
        if(pair_.first == playerAccount)
            return pair_.second;
        if(pair_.second == playerAccount)
            return pair_.first;
    }

    return 0;
}

void GameServer::removePlayingPairs(const qint64 playerAccount){
    for(int i = 0; i < playing_pairs.size(); i ++){
        if(playing_pairs[i].first == playerAccount || playing_pairs[i].second == playerAccount){
            playing_pairs.remove(i);
            return;
        }
    }
}


void GameServer::dealPlayerLogined(const qint64 playerAccount){
    GameClientWork* _work = (GameClientWork*)QObject::sender();
    this->players[playerAccount] = _work;

    emit this->transferPlayerLogined(playerAccount);
}

void GameServer::dealJoinMatching(const qint64 playerAccount){
    QMutexLocker locker(&this->mutex);
    if(this->matching_queue.size() >= 1){
        qint64 oth_player = matching_queue.front();
        matching_queue.pop_front();
        playing_pairs.push_back(QPair<qint64,qint64> (playerAccount,oth_player));
        emit this->transferMatchSucess(playerAccount,oth_player);
        return;
    }
    else if(this->matching_queue.size() < 1){
        matching_queue.push_back(playerAccount);
        emit this->transferWaitingForMatching(playerAccount);
        return;
    }
}

void GameServer::dealOneQuited(const qint64 playerAccount){
    qint64 winnerAccount = FindOtherBattler(playerAccount);
    qDebug() << playerAccount << "退出, " << winnerAccount << "获胜";
    this->removePlayingPairs(playerAccount);
    emit this->PlayerSucceed_Quit(winnerAccount);
}

void GameServer::dealOneFinished(const qint64 playerAccount){
    qint64 winnerAccount = playerAccount;
    qint64 loserAccount = FindOtherBattler(winnerAccount);
    qDebug() << winnerAccount << "完成 " << loserAccount << "失败";
    this->removePlayingPairs(winnerAccount);
    emit this->PlayerSucceed_Win(winnerAccount);
    emit this->PlayerLose(loserAccount);
}
