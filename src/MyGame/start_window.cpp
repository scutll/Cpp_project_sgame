#include "start_window.h"
#include "ui_start_window.h"
#include "mainwindow.h"
#include <QMessageBox>
#include "player_connect.h"
#include <QListView>
#include <QToolButton>
#include <QLineEdit>
#include <QInputDialog>


start_window::start_window(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::start_window),
    model(new QStandardItemModel(this))
{
    ui->msg_list_window = new QListView;
    ui->RetryConnectionBtn = new QPushButton;
    matching = false;
    ui->setupUi(this);
    ui->msg_list_window->setModel(model);
    setWindowTitle(tr("活字飞花"));
    w = new MainWindow;
    w->init();

    connect(w,SIGNAL(GameClosed()),this,SLOT(gameClosed()));


    ui->SendBtn = new QPushButton;



    ui->MessageLine->setPlaceholderText("输入消息......");
    ui->ReceiverLine->setPlaceholderText("发送至: ");
    //这俩按钮不能进行new，不然不能更新文本
    // ui->MessageLine = new QEditLine;
    // ui->LoginBtn = new QPushButton;


}

start_window::~start_window()
{
    delete ui;
}

void start_window::on_offline_game_clicked()
{
    this->close();
    w->OfflineMode();
    w->show();
    w->generate_map();
}


void start_window::on_online_game_clicked()
{
    // qDebug()<<"matching: "<<(matching ? "true" : "false");
    // if(matching)
    //     return;

    // matching = true;
    // // QMessageBox msg;
    // // msg.setText(tr("匹配中......"));
    // // msg.information(this,nullptr,tr("匹配中......"));
    // msg_os("匹配中......");
    // QApplication::processEvents();//强制处理事件队列中的消息，因为conncet函数里的waitfortimeout是阻塞调用，此时msg_os会被阻塞留在事件队列中无法运行
    // this->GameServer = new PlayerConnector;

    // bool connect_success;
    // connect_success = GameServer->connect_to_server();
    // if(!connect_success){
    //     qDebug()<<"connection failed";
    //     msg_os("连接服务器失败", true);
    //     // if(msg.isVisible())
    //     //     msg.close();
    //     matching = false;
    //     return;
    // }

    // GameServer->send_match_request();

    if (matching)
        return;

    matching = true;
    msg_os("正在连接游戏服务器......");

    this->GameServer = new PlayerConnector;



    connect(GameServer, &PlayerConnector::connectionSucceeded, this, &start_window::onConnectionSucceeded);
    connect(GameServer, &PlayerConnector::connectionFailed, this, &start_window::onConnectionFailed);

    GameServer->connect_to_server();


    connect(GameServer,&PlayerConnector::recv_msg_str,this,&start_window::onRecv_msg_str);
    connect(GameServer,&PlayerConnector::recv_msg_pakcage,this,&start_window::onRecv_msg_package);


}

void start_window::on_Awake_signal(){
    w->close();
    qDebug()<<"close game window";
    this->show();
    qDebug()<<"awake start window";
}

void start_window::app_msg(const QString& sender,const QString& message,bool error){
    QString messageLine = "[" + sender + "] " + message;

    QStandardItem *item = new QStandardItem(messageLine);
    if(error)
        item->setForeground(QBrush(QColor("red")));
    model->appendRow(item);

    QModelIndex lastIndex = model->index(model->rowCount() - 1, 0);
    ui->msg_list_window->scrollTo(lastIndex);
}

void start_window::add_msg(const QString msg,bool error){
    QStandardItem *item = new QStandardItem(msg);
    if(error)
        item->setForeground(QBrush(QColor("red")));
    model->appendRow(item);

    QModelIndex lastIndex = model->index(model->rowCount() - 1, 0);
    ui->msg_list_window->scrollTo(lastIndex);

}

void start_window::msg_server(const QString msg){
    add_msg("[服务器]"+msg);
}
void start_window::msg_os(const QString msg,bool error){
    add_msg("[系统]"+msg,error);
}

void start_window::onConnectionSucceeded() {
    qDebug() << "Connection succeeded!";
    msg_os("连接服务器成功，正在匹配：");
    GameServer->send_match_request();
    // GameServer->send_msg("hello");
}

void start_window::onConnectionFailed(const QString &errorMessage) {
    qDebug() << "Connection failed:" << errorMessage;
    msg_os(errorMessage, true);
    matching = false;
}

void start_window::onRecv_msg_str(const QString& msg){
    msg_server(msg);
}

void start_window::onRecv_msg_package(const package& pkg){
    msg_os("匹配成功,正在进入游戏");

    for(int i=0;i<36;i++){
        msg_os(QString::number(pkg.map[i].value));
    }

    this->close();

    w->show();
    w->generate_map();

    w->load_game_and_generate(pkg);
}

void start_window::gameClosed(){
    this->show();
}

// void start_window::dealReceiverSet(const QString &receiverName) {
//     this->receiverName_temp = receiverName;
//     // ui->ReceiverMenuBtn->setText(receiverName_temp);

//     qDebug() << "receiverName changed: " << receiverName_temp;
// }



// void start_window::on_ReceiverMenuBtn_clicked()
// {


//     if(!menu->isVisible()){
//         QPoint pos = mapToGlobal(ui->ReceiverMenuBtn->pos() + QPoint(0,646));
//         menu->popup(pos);
//         menuON = true;
//     }
//     else{
//         menu->hide();
//         menuON = false;
//     }
// }


void start_window::on_SendBtn_clicked()
{
    if(!clientConnected){
        app_msg("系统","网络未连接! 请先登录",true);
        return;
    }
    if(ui->ReceiverLine->text() == this->chatclient->userName_() || ui->ReceiverLine->text() == "")
        return;

    qDebug() << ui->ReceiverLine->text() << " sending message : " << ui->MessageLine->text();
    this->chatclient->INTERFACE_SendUserMessage(chatclient->userName_(),ui->ReceiverLine->text(),ui->MessageLine->text());

}


void start_window::on_LoginBtn_clicked()
{
    if(!userLoged){
        bool ok;
        QString username = QInputDialog::getText(
            this,
            tr("输入用户名以登录"),
            tr("your user name: "),
            QLineEdit::Normal,
            "",
            &ok
            );
        if(ok && username != ""){
            //假设不会有重名的
            this->userName = username;
            chatclient = new Client(this->userName,0);
            this->init_chatclient();
            qDebug() << "userName set: " << userName;
            this->userLoged = true;
            chatclient->INTERFACE_dealUserLogined(this->userName);

            ui->LoginBtn->setText("已登录: " + userName);
            ui->LoginBtn->repaint();  // 强制立即重绘，不经过事件队列
            qDebug() << ui->LoginBtn->text();
            clientConnected = true;
        }
        else{
            msg_os("请输入正确用户名",true);
            return;
        }
    }
    else{
        app_msg("系统","您已登录! 用户名: " + this->userName,true);
    }
}


void start_window::on_RetryConnectionBtn_clicked()
{
    if(clientConnected)
        chatclient->INTERFACE_retryConnect();
}


void start_window::dealINTERFACE_dealAcceptNormalMessage(const QString& senderName,const QString& message){
    app_msg(senderName,message);
}

void start_window::dealINTERFACE_dealUserDisconnected(const QString& userName){
    QString message = userName + "已离线";

    // for(auto& action : menu->actions()){
    //     if(action->text() == userName){
    //         menu->removeAction(action);
    //         break;
    //     }
    // }
    app_msg("系统",message,true);
}

void start_window::dealINTERFACE_dealUserLogined(const QString& userName){
    if(userName == this->chatclient->userName_())
        return;

    QString message = userName + "已上线";
    // menu->addAction(userName,[this,userName](){this->receiverName_temp = userName; qDebug() << "Receiver Switched To" << userName;});
    app_msg("系统",message);
}

void start_window::init_chatclient(){
    connect(chatclient,&Client::INTERFACE_dealUserDisconnected,this,&start_window::dealINTERFACE_dealUserDisconnected);
    connect(chatclient,&Client::INTERFACE_dealUserLogined,this,&start_window::dealINTERFACE_dealUserLogined);
    connect(chatclient,&Client::INTERFACE_dealAcceptNormalMessage,this,&start_window::dealINTERFACE_dealAcceptNormalMessage);
}
