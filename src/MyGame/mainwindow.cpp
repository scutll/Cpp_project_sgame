#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <vector>
#include <iostream>
#include <string>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <QString>
#include "../../src/TerminalVersion/common.h"
#include "scene.h"
#include "../../src/TerminalVersion/language.h"
#include <chrono>
#include "../../src/TerminalVersion/save_package.h"
#include <ctime>
#include <QString>
#include "archives.h"
#include <QInputDialog>
#include <QApplication>
#include "../../src/TerminalVersion/utility.inl"

Scene game;
// 使用两个map。一个map_索引对应按钮，即坐标->按钮。另一个_map按钮对应索引，即btn->text().toInt() -> 坐标
std::vector<point_> _map(37);        // 数字所在的位置，点到按钮时用于查询改按钮的位置
std::vector<QPushButton *> map_(36); // Button的地图

// 这俩在游戏中不用更新，是固定的
std::vector<QPushButton *> blocks(36);
std::vector<point_> blocks_pos(36);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    isUntitled = true;
    ui->setupUi(this);
    setWindowTitle(curFile);

    // 一个总是存在的存档窗口
    this->arc = new Archives;
    start_time_ = now_time();
    OfflineMode();
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
    // 将Button作为块存入到blocks
    for (int i = 0; i < 36; i++)
    {
        std::string obj_name = "b" + std::to_string(i + 1);
        QPushButton *btn = findChild<QPushButton *>(obj_name);
        blocks[i] = (btn);
        // 保存第i个方格的坐标
        //  blocks_pos.push_back(btn->mapToGlobal(btn->pos()));
        blocks_pos[i] = (point_(btn->pos().x(), btn->pos().y()));
    }

    // 批量绑定槽函数
    for (int i = 0; i < 36; i++)
    {
        QPushButton *btn = blocks[i];
        connect(btn, &QPushButton::clicked, btn, [this, btn]
                { setPoint(btn); });
    }

    //绑定存档载入信号
    connect(arc, &Archives::loadFinished, this, &MainWindow::on_loadFinished);

}


void MainWindow::generate_map()
{

    bool safe_block = true;
    // 检查
    for (int i = 0; i < 36; i++)
    {
        if (blocks[i] == nullptr)
            safe_block = false;
    }

    if (safe_block)
    {
        for (int i = 0; i < 36; i++)
        {
            int num = game.map[i].value - 1;
            QPoint point = QPoint(blocks_pos[i].x, blocks_pos[i].y);
            // 更新_map和map_
            blocks[num]->move(point);
            map_[i] = blocks[num];
            _map[num + 1] = point_(i % 6, i / 6);
        }
    }

    start_time_ = now_time();
}

// 两个Button交换
void MainWindow::swap_point(QPushButton *btn1, QPushButton *btn2)
{
    QPoint tmp_point = QPoint(btn1->pos().x(), btn1->pos().y());
    btn1->move(QPoint(btn2->pos().x(), btn2->pos().y()));
    btn2->move(tmp_point);
}

// 按照map_上的坐标互换
void MainWindow::swap_point(point_ p1, point_ p2)
{
    // 在map_上交换
    QPushButton *btn1 = map_[p1.x + p1.y * 6];
    QPushButton *btn2 = map_[p2.x + p2.y * 6];

    map_[p1.x + p1.y * 6] = btn2;
    map_[p2.x + p2.y * 6] = btn1;
    // 窗口里交换
    swap_point(btn1, btn2);

    // 记录格子坐标的数组更新
    point_ tmp_point = _map[btn1->text().toInt()];
    _map[btn1->text().toInt()] = _map[btn2->text().toInt()];
    _map[btn2->text().toInt()] = tmp_point;
}

// 记录setPoint被调用的次数，每点击两次即调用两次setPoint就要变换一次然后重置
int counter = 0;

void MainWindow::setPoint(QPushButton *btn)
{
    point_ point = _map[btn->text().toInt()];
    game.setPoint(point.x, point.y);
    // std::cout<<point.x<<" "<<point.y<<"            "<<game.curPoint()->x<<" "<<game.curPoint()->y<<std::endl;
    btn->setStyleSheet("color: red");
    game.Switch_point();
    counter++;
    if (counter >= 2)
    {
        game.execute();

        // 重置
        for (int i = 0; i < 36; i++)
        {
            map_[i]->setStyleSheet("color: black;");
        }

        counter = 0;
    }
}


void MainWindow::on_save_game_triggered()
{
    // save();

    // 存档界面
    arc->toSave();
    arc->flash();
    arc->show();
}

void MainWindow::on_load_game_triggered()
{

    arc->toLoad();
    arc->flash();
    arc->show();

    qDebug() << "now can generate";
}

void save_game(QString path, const char *name);
package load_game(QString path);
bool AskforOverrideArchive();
QString AskforArchiveName();
bool confirmSave();

void Archives::Archives_name_clicked(QPushButton *btn, QLabel *time_label)
{
    qDebug()<<"Clicked for archives!";

    // 窗口用于保存的情况
    if (this->to_save)
    {

        // 点击已经存在的存档，覆盖
        bool replace = false;
        if (btn->text() != QString("空存档"))
        {
            replace = true;
            qDebug() << "replace";
        }

        QString archive_name;
        // 空存档询问存档名
        if (!replace)
        {
            archive_name = AskforArchiveName();
        }
        else if (replace)
        {
            //点击已有存档，询问是否覆盖，覆盖则询问存档名
            replace = AskforOverrideArchive();
            if(!replace)
                archive_name = btn->text();
            else{       //覆盖，询问存档名
                archive_name = AskforArchiveName();
            }
        }

        QString filename = path + archive_name + QString(".game"); // path


        //确认存档
        bool confirm = confirmSave();
        if(!confirm)
            return;

        //覆盖存档且确认存档时要删掉旧存档
        if(replace){
            QString old_file = path + btn->text() + ".game";
            QFile::remove(old_file);
            qDebug()<<"remove file: "<<old_file;
        }

        // 更新存档名和时间
        btn->setText(archive_name);
        std::tm time = now_time();
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time);
        QString arc_time = QString::fromUtf8(buffer);
        qDebug() << arc_time << " time";
        time_label->setText(arc_time);


        //进行保存
        qDebug() << "Saveing: " << filename;
        save_game(filename, archive_name.toStdString().c_str());
        if (!replace)
            archives_cnt++;
        qDebug() << "saved";
    }



    // 窗口用于载入的情况
    else if (!this->to_save)
    {
        //检查是否点击了空存档
        if(btn->text() == QString("空存档")){
            return ;
        }


        QString filename = path + btn->text() + QString(".game");


        qDebug() << "Loading: " << filename;
        pkg_forload = load_game(filename);

        for (int i = 0; i < 36; i++)
        {
            game.map[i] = pkg_forload.map[i];
        }
        qDebug() << "load success";

        emit loadFinished();

    }
}

bool confirmSave(){
    QMessageBox confirmbox;
    confirmbox.setWindowTitle("确认存档");
    confirmbox.setText("是否确认保存存档");
    confirmbox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    confirmbox.setDefaultButton(QMessageBox::No);

    int result = confirmbox.exec();

    if (result == QMessageBox::Yes) {
        qDebug() << "replace the archive";
        return true; // 返回 true 表示用户同意覆盖
    } else {
        qDebug() << "do not replace";
        return false; // 返回 false 表示用户不同意覆盖
    }
}

QString AskforArchiveName(){
    bool *ok = new bool;
    QString archive_name = QInputDialog::getText(nullptr, QObject::tr("请输入存档名"),
                                         QObject::tr("你的存档名"), QLineEdit::Normal, QObject::tr("newgame"), ok);
    qDebug() << "inputing";
    if ((*ok) && !archive_name.isEmpty())
    {
        qDebug() << "输入存档名: " << archive_name;
    }
    else
    {
        archive_name = QString("newgame");
    }

    return archive_name;
}

bool AskforOverrideArchive(){
    QMessageBox confirmbox;
    confirmbox.setWindowTitle("覆盖存档");
    confirmbox.setText("存档已存在，是否覆盖");
    confirmbox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    confirmbox.setDefaultButton(QMessageBox::No);

    int result = confirmbox.exec();

    if (result == QMessageBox::Yes) {
        qDebug() << "replace the archive";
        return true; // 返回 true 表示用户同意覆盖
    } else {
        qDebug() << "do not replace";
        return false; // 返回 false 表示用户不同意覆盖
    }
}


void MainWindow::load_game_and_generate(const package& pkg){

    for (int i = 0; i < 36; i++)
    {
        game.map[i] = pkg.map[i];
    }
    qDebug() << "load success";

    generate_map();
}

void MainWindow::on_loadFinished()
{
    generate_map();
}

void save_game(QString path, const char *name)
{
    package pkg;

    // 存档名
    memcpy(pkg.archive_name, name, sizeof(pkg.archive_name));

    // 获取保存时间
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_now = *std::localtime(&now_time);
    pkg.date = local_now;

    for (int i = 0; i < 36; i++)
    {
        pkg.map[i] = game.map[i];
    }

    std::ofstream fout(path.toStdString(), std::ios::binary);
    if (!fout.is_open())
    {
        QMessageBox::warning(nullptr, nullptr, ("warning"), ("保存失败! 无法打开文件"));
    }
    fout.seekp(std::ios::beg);

    fout.write(reinterpret_cast<char *>(&pkg), sizeof(package));
    fout.close();
}

package load_game(QString path)
{

    package pkg;

    std::fstream fin(path.toStdString(), std::ios::binary | std::ios::in);
    if (!fin.is_open())
    {
        QMessageBox::warning(nullptr, nullptr, "warning", "无法打开文件!");
    }

    fin.seekg(std::ios::beg);

    fin.read(reinterpret_cast<char *>(&pkg), sizeof(package));

    fin.close();
    return pkg;
}

//游戏完成的判断
void MainWindow::on_Finish_button_clicked()
{
    bool Finished = game.isComplete();

    if(Finished){
        Victory_Settlement();
    }
    else{
        QMessageBox::information(this,"Not Finished Yet!","你还没有完成");
        }
}

void MainWindow::Victory_Settlement(){

    std::tm now;
    now = now_time();

    std::time_t start = std::mktime(&start_time_);
    std::time_t finish = std::mktime(&now);

    double diff_seconds = difftime(finish,start);

    int diff_minutes = static_cast<int>(diff_seconds / 60) % 60;
    int diff_hours = static_cast<int>(diff_seconds / 3600) % 60;
    int diff_sec = static_cast<int>(diff_seconds) % 60;
    QMessageBox::information(this,"Congratulations",QString("恭喜你！完成了！\n 总时长：%1小时 %2分钟 %3秒 \n 即将退出到开始窗口").arg(diff_hours)
                                                          .arg(diff_minutes).arg(diff_sec));
    if(!is_online()){

        qDebug()<<"send awake signal:";
        emit Awake_StartWindow();
    }
    else if(is_online()){
        emit finish_online(this->playerAccount);   //待完善
    }
}


void MainWindow::OnlineMode(){
    online = true;
}


void MainWindow::OfflineMode(){
    online = false;
}

bool MainWindow::is_online(){
    return online;
}


void MainWindow::GameLoginSuccess(const qint64 playerAccount){
    this->playerAccount = playerAccount;
    this->setWindowTitle(QString::number(this->playerAccount));

    emit this->app_msg_Signal("game服务器", "登录成功");
}

void MainWindow::gameStart(const qint64 playerAccount,const qint64 oth_player, const package& newGame){
    if(this->playerAccount != playerAccount){
        qDebug() << "用户名不一致!" << this->playerAccount << " != " << playerAccount;
        return;
    }

    this->othPlayer = oth_player;
    load_game_and_generate(newGame);

    emit app_msg_Signal("游戏服务器", "匹配成功，游戏开始，对战玩家: " + QString::number(this->othPlayer),false);
    emit noticeGameStarted();
    qDebug() << "游戏开始";


}

void MainWindow::gameLose(const qint64 loserAccount){
    emit app_msg_Signal("游戏服务器", QString::number(this->othPlayer) + " 先完成，对战失败!",false);
    qDebug() << "对方先完成，失败";
}

void MainWindow::gameWin(const qint64 winnerAccount){
    emit app_msg_Signal("游戏服务器", "完成游戏，胜利! ",false);
    qDebug() << "率先完成游戏，成功";
}

void MainWindow::gameWinQuit(const qint64 playerAccount){
    emit app_msg_Signal("游戏服务器", QString::number(this->othPlayer) + " 已退出游戏，您取得胜利",false);
    qDebug() << "对方退出，胜利";
}

