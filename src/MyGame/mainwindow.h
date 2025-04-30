#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QPushButton>
#include <QMainWindow>
#include "../../src/common.h"
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


    // bool save();
    // bool saveAs();
    // bool saveFile(const QString &filename);
    // bool loadFile(const QString &filename);

signals:
    void Awake_StartWindow();


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

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
