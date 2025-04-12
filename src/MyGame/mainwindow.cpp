#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <vector>
#include <string>
#include <iostream>
#include "../../src/common.h"
#include "../../src/scene.h"
#include "../../src/language.h"
// #include "../../src/block.h"

Scene game;
std::vector<QPushButton*> blocks(36);
std::vector<point_> blocks_pos(36);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_b1_clicked()
{
    ui->b1->setStyleSheet("color: red;");
}

void MainWindow::init()
{
    //将Button作为块存入到blocks
    for(int i = 0;i<36;i++){
        std::string obj_name = "b" + std::to_string(i+1);
        QPushButton* btn = findChild<QPushButton*>(obj_name);
        blocks[i] = (btn);
        //保存第i个方格的坐标
        // blocks_pos.push_back(btn->mapToGlobal(btn->pos()));
        blocks_pos[i] = (point_(btn->pos().x(),btn->pos().y()));
    }


}

void MainWindow::generate_map(){

    I18n::Instance().setLanguage(Language::ENGLISH);
    game.SetMode(KeyMode::NORMAL);
    game.generate();

    bool safe_block = true;

    for(int i=0;i<36;i++){
        if(blocks[i]==nullptr)
            safe_block = false;
    }

    if(safe_block)
    {
        for(int i=0;i<36;i++){
            int num = game.map[i].value - 1;
            QPoint point = QPoint(blocks_pos[i].x,blocks_pos[i].y);
            std::cout<<point.x()<<point.y()<<std::endl;
            blocks[num]->move(point);
            std::cout<<"Next"<<std::endl;
        }
    }

}
