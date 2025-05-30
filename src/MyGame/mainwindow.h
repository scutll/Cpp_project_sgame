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

signals:
    void Awake_StartWindow();

    void GameClosed();

    void finish_online();

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


protected:
    void closeEvent(QCloseEvent* event) override{
        qDebug()<<"main window closed";

        emit GameClosed();
    }
};
#endif // MAINWINDOW_H
