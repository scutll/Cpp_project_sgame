#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <vector>
#include <iostream>
#include <string>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <QString>
#include "../../src/common.h"
#include "scene.h"
#include "../../src/language.h"
#include <chrono>
#include "../../src/save_package.h"
#include <ctime>

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
    isUntitled = true;
    ui->setupUi(this);
    setWindowTitle(curFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::init()
{
    game.generate();


    I18n::Instance().setLanguage(Language::ENGLISH);
    game.SetMode(KeyMode::NORMAL);

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

    bool safe_block = true;
    //检查
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
    // std::cout<<point.x<<" "<<point.y<<"            "<<game.curPoint()->x<<" "<<game.curPoint()->y<<std::endl;
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

void save_game(QString path,const char* name);

bool MainWindow::save(){
    if(isUntitled)
        return saveAs();
    else
        return saveFile(curFile);
}

bool MainWindow::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("另存为"),"newgame.game");
    if(fileName.isEmpty()) return false;
    return saveFile(fileName);
}
bool MainWindow::saveFile(const QString &filename){
    const char* name = "mxl";  //以后可以设置一个弹窗询问存档名

    //鼠标指针变成等待状态
    QApplication::setOverrideCursor(Qt::WaitCursor);
    save_game(filename,name);   //保存
    //鼠标指针恢复
    QApplication::restoreOverrideCursor();
    isUntitled = false;
    //获得文件标准路径
    curFile = QFileInfo(filename).canonicalFilePath();
    setWindowTitle(curFile);
    return true;
}

package load_game(QString path);

bool MainWindow::loadFile(const QString &filename){
    package archive = load_game(filename);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    //载入
    for(int i = 0; i < 36; i++){
        game.map[i] = archive.map[i];
    }
    //可以设置弹窗表示保存时间等
    QApplication::restoreOverrideCursor();

    curFile = QFileInfo(filename).canonicalFilePath();
    setWindowTitle(curFile);
    return true;
}


void MainWindow::on_save_game_triggered(){
    save();
}


void MainWindow::on_load_game_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("载入"),curFile);
    if(fileName.isEmpty()){
        QMessageBox::warning(nullptr,nullptr,"warning","文件无效");
    }

    loadFile(fileName);
    generate_map();
}

void save_game(QString path,const char* name){
    package pkg;

    //存档名
    memcpy(pkg.player_name,name,sizeof(pkg.player_name));

    //获取保存时间
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_now = *std::localtime(&now_time);
    pkg.date = local_now;

    for(int i = 0;i < 36; i ++){
        pkg.map[i] = game.map[i];
    }

    std::ofstream fout(path.toStdString(),std::ios::binary);
    if(!fout.is_open()){
        QMessageBox::warning(nullptr,nullptr,("warning"),("保存失败! 无法打开文件"));
    }
    fout.seekp(std::ios::beg);

    fout.write(reinterpret_cast<char*>(&pkg),sizeof(package));
    fout.close();
}

package load_game(QString path){
    package pkg;

    std::fstream fin(path.toStdString(),std::ios::binary | std::ios::in);
    if(!fin.is_open()){
        QMessageBox::warning(nullptr,nullptr,"warning","无法打开文件!");
    }

    fin.seekg(std::ios::beg);

    fin.read(reinterpret_cast<char*>(&pkg),sizeof(package));

    fin.close();
    return pkg;
}
