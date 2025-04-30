#include "start_window.h"
#include "ui_start_window.h"
#include "mainwindow.h"
#include <QMessageBox>


start_window::start_window(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::start_window)
{
    ui->setupUi(this);
    setWindowTitle(tr("活字飞花"));
    w = new MainWindow;

}

start_window::~start_window()
{
    delete ui;
}

void start_window::on_offline_game_clicked()
{
    this->close();
    MainWindow* w = new MainWindow;
    w->init();
    w->show();
    w->generate_map();
}


void start_window::on_online_game_clicked()
{
    QMessageBox msg;
    msg.setText(tr("匹配中......"));
    msg.information(this,nullptr,tr("匹配中......"));
}

void start_window::on_Awake_signal(){
    w->close();
    qDebug()<<"close game window";
    this->show();
    qDebug()<<"awake start window";
}
