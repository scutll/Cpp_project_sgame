#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include <QTcpSocket>
#include <QObject>

class BattleHandler : public QObject {
    Q_OBJECT

    QTcpSocket* player1, * player2;

public:
    explicit BattleHandler(QTcpSocket* player1, QTcpSocket* player2, QObject* parent = nullptr);

public slots:
    void startBattle();
    void handlePlayer1();
    void handlePlayer2();
    void handlePlayer(QTcpSocket* player, QTcpSocket* opponent);

signals:
    void finished();
};

#endif // BATTLE_HANDLER_H