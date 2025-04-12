#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <vector>
#include <iostream>
#include <string>
#include "../../src/common.h"
#include "scene.h"
#include "../../src/language.h"
// #include "../../src/block.h"

Scene game;
//使用两个map。一个map_索引对应按钮，即坐标->按钮。另一个_map按钮对应索引，即btn->text().toInt() -> 坐标
std::vector<point_> _map(37);   // 数字所在的位置，点到按钮时用于查询改按钮的位置
std::vector<QPushButton*> map_(36);  //Button的地图
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


void MainWindow::init()
{
    game.generate();

    game.bind_Window(this);
    //将Button作为块存入到blocks
    for(int i = 0;i<36;i++){
        std::string obj_name = "b" + std::to_string(i+1);
        QPushButton* btn = findChild<QPushButton*>(obj_name);
        blocks[i] = (btn);
        //保存第i个方格的坐标
        // blocks_pos.push_back(btn->mapToGlobal(btn->pos()));
        blocks_pos[i] = (point_(btn->pos().x(),btn->pos().y()));
    }

    //批量绑定槽函数
    for(int i = 0;i < 36; i++){
        QPushButton* btn = blocks[i];
        connect(btn,&QPushButton::clicked,btn,[this,btn]{setPoint(btn);});
    }
}

void MainWindow::generate_map(){

    I18n::Instance().setLanguage(Language::ENGLISH);
    game.SetMode(KeyMode::NORMAL);


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
            blocks[num]->move(point);
            map_[i] = blocks[num];
            _map[num+1] = point_(i%6,i/6);
        }

    }

}

//两个Button交换
void MainWindow::swap_point(QPushButton* btn1,QPushButton* btn2){
    QPoint tmp_point = QPoint(btn1->pos().x(),btn1->pos().y());
    btn1->move(QPoint(btn2->pos().x(),btn2->pos().y()));
    btn2->move(tmp_point);
}

//按照map_上的坐标互换
void MainWindow::swap_point(point_ p1,point_ p2){
    //在map_上交换
    QPushButton* btn1 = map_[p1.x + p1.y*6];
    QPushButton* btn2 = map_[p2.x + p2.y*6];

    map_[p1.x + p1.y*6] = btn2;
    map_[p2.x + p2.y*6] = btn1;
    //窗口里交换
    swap_point(btn1,btn2);

    //记录格子坐标的数组更新
    point_ tmp_point = _map[btn1->text().toInt()];
    _map[btn1->text().toInt()] = _map[btn2->text().toInt()];
    _map[btn2->text().toInt()] = tmp_point;

}


//记录setPoint被调用的次数，每点击两次即调用两次setPoint就要变换一次然后重置
int counter = 0;

void MainWindow::setPoint(QPushButton* btn){
    point_ point = _map[btn->text().toInt()];
    game.setPoint(point.x,point.y);
    std::cout<<point.x<<" "<<point.y<<"            "<<game.curPoint()->x<<" "<<game.curPoint()->y<<std::endl;
    btn->setStyleSheet("color: red");
    game.Switch_point();
    counter++;
    if(counter >= 2){
        game.execute();

        //重置
        for(int i = 0; i <36; i++){
            map_[i]->setStyleSheet("color: black;");
        }

        counter = 0;
    }
}
