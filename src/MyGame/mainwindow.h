#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QPushButton>
#include <QMainWindow>
#include "../../src/TerminalVersion/common.h"
#include "archives.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();
    void generate_map();
    void swap_point(point_ p1,point_ p2);
    void on_loadFinished();
    std::tm start_time();
    void Victory_Settlement();

    void load_game_and_generate(const package& pkg);

    void OnlineMode();
    void OfflineMode();
    bool is_online();
    // bool save();
    // bool saveAs();
    // bool saveFile(const QString &filename);
    // bool loadFile(const QString &filename);

    void gameStart(const qint64 playerAccount, const qint64 othPlayer, const package& GamePkg);

    void gameLose(const qint64 loserAccount);

    void gameWin(const qint64 winnerAccount);

    void gameWinQuit(const qint64 playerAccount);

    void GameLoginSuccess(const qint64 playerAccount);


signals:
    void noticeGameStarted();

    void Awake_StartWindow();

    void onlineGameClosed();

    void offlineGameClosed();

    void GameEnded(bool win);

    void finish_online(const qint64 playerAccount);

    void app_msg_Signal(const QString& from, const QString& message, bool warning = false);

private slots:
    void setPoint(QPushButton* btn);

    void on_save_game_triggered();

    void on_load_game_triggered();


    void on_Finish_button_clicked();

private:
    void swap_point(QPushButton* btn1,QPushButton* btn2);
    bool isUntitled;
    QString curFile;
    Archives* arc;
    std::tm start_time_;

    bool online;
    Ui::MainWindow *ui;
    qint64 playerAccount = 0;
    qint64 othPlayer = 0;


protected:
    void closeEvent(QCloseEvent* event) override{
        qDebug()<<"main window closed";

        if(is_online()){
            qDebug() << "网络模式退出: ";
            emit this->onlineGameClosed();
            return;
        }
        else{
            emit this->offlineGameClosed();
            return;
        }
    }
};
#endif // MAINWINDOW_H
