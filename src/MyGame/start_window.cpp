#include "start_window.h"
#include "ui_start_window.h"
#include "mainwindow.h"
#include <QMessageBox>
#include "player_connect.h"
#include <QListView>
#include <QToolButton>
#include <QLineEdit>
#include <QInputDialog>
#include <QDir>
#include "asklogin.h"


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


    QDir dir(QCoreApplication::applicationDirPath());
    if(!dir.exists("archives"))
        dir.mkdir("archives");
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
    if (!chatclient->INTREFACE_isConnected()) {
        app_msg("系统","服务器已断开连接......发送失败",true);
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
        qint64 userAccount;
        QString userPassword;
        AskLogin* loginDialog = new AskLogin(this);
        qDebug() << "正在打开登录界面";
        if(loginDialog->exec() == QDialog::Accepted){
            userAccount = loginDialog->getUserAccount();
            userPassword = loginDialog->getUserPassword();

            this->temp_password = userPassword;

            chatclient = new Client(QString::number(userAccount),userAccount);
            this->init_chatclient();
            clientConnected = true;
            chatclient->INTERFACE_LoginRequest(userAccount,userPassword);
        }
        else{
            msg_os("登录失败，请输入正确账号密码",true);
            return;
        }

    }
    else{
        bool ok;
        QString newName = QInputDialog::getText(this,
                                                "修改名称",
                                                "请输入您的新名称:",
                                                QLineEdit::Normal,
                                                "",
                                                &ok);
        if(ok && !newName.isEmpty()){
            chatclient->INTERFACE_UserNameModifyRequest(GLOB_UserAccount,newName);
        }
        else if(!ok){
            qDebug() << "用户取消修改名称";
        }
        else{
            app_msg("系统","名称不合法",true);
        }
    }
}


void start_window::on_RetryConnectionBtn_clicked()
{
    if(clientConnected){
        app_msg("系统","正在尝试重连");
        chatclient->INTERFACE_retryConnect(GLOB_UserAccount,this->temp_password);
    }
}


void start_window::dealINTERFACE_dealAcceptNormalMessage(const QString& senderName,const QString& message){
    app_msg(senderName,message);
}

void start_window::dealINTERFACE_dealUserDisconnected(const QString& userName){
    QString message = userName + "已离线";
    app_msg("系统",message,true);
}

void start_window::dealINTERFACE_dealConnnectError(const QString& error) {
    this->app_msg("系统","连接失败: " + error, true);
}

void start_window::dealINTERFACE_dealUserLogined(const QString& userName){
    if(userName == this->chatclient->userName_())
        return;

    QString message = userName + "已上线";
    // menu->addAction(userName,[this,userName](){this->receiverName_temp = userName; qDebug() << "Receiver Switched To" << userName;});
    app_msg("系统",message);
}

void start_window::dealINTERFACE_dealServerDisconnected() {
    app_msg("系統","服务器已断开，请稍后尝试重新连接",true);
}


void start_window::dealINTERFACE_dealLoginAccepted(const qint64 userAccount,const QString& userName) {
    this->userLoged = true;
    GLOB_IsConnectedToServer = true;
    GLOB_UserAccount = userAccount;
    GLOB_UserName = userName;

    ui->LoginBtn->setText("已登录: (" + QString::number(userAccount) + ") " + userName);
    ui->LoginBtn->repaint();  // 强制立即重绘，不经过事件队列

    app_msg("系统","登录成功: " + userName,false);

}

void start_window::dealINTERFACE_dealRefusedWrongPsw(const qint64 userAccount) {
    this->userLoged = false;
    GLOB_IsConnectedToServer = true;
    GLOB_UserAccount = userAccount;

    app_msg("系统","登录失败,密码错误" + userName,true);
}

void start_window::dealINTERFACE_dealNameModifyAccepted(const qint64 userAccount, const QString &newName) {
    GLOB_UserName = newName;
    app_msg("系统","名称修改成功，新名称: " + newName);
    ui->LoginBtn->setText("已登录: (" + QString::number(userAccount) + ") " + GLOB_UserName);
}

void start_window::dealINTERFACE_repeatedName() {
    app_msg("系统","修改名称失败: 该名称已被注册",true);
}

void start_window::dealINTERFACE_dealNoticeUserNameModified(const qint64 userAccount, const QString &userName, const QString &newName) {
    app_msg("系统","(" + QString::number(userAccount) + ")" + userName + " 修改昵称为: " + newName);
}





void start_window::init_chatclient(){
    connect(chatclient,&Client::INTERFACE_dealUserDisconnected,this,&start_window::dealINTERFACE_dealUserDisconnected);
    connect(chatclient,&Client::INTERFACE_dealUserLogined,this,&start_window::dealINTERFACE_dealUserLogined);
    connect(chatclient,&Client::INTERFACE_dealAcceptNormalMessage,this,&start_window::dealINTERFACE_dealAcceptNormalMessage);
    connect(chatclient,&Client::INTERFACE_dealConnnectError,this,&start_window::dealINTERFACE_dealConnnectError);
    connect(chatclient,&Client::INTERFACE_ServerDisconnected,this,&start_window::dealINTERFACE_dealServerDisconnected);
    connect(chatclient,&Client::INTERFACE_RefusedWrongPsw,this,&start_window::dealINTERFACE_dealRefusedWrongPsw);
    connect(chatclient,&Client::INTERFACE_LoginAccepted,this,&start_window::dealINTERFACE_dealLoginAccepted);
    connect(chatclient,&Client::INTERFACE_NameModifyAccepted,this,&start_window::dealINTERFACE_dealNameModifyAccepted);
    connect(chatclient,&Client::INTERFACE_NoticeUserNameModified,this,&start_window::dealINTERFACE_dealNoticeUserNameModified);
    connect(chatclient,&Client::INTERFACE_repeatedName,this,&start_window::dealINTERFACE_repeatedName);

}
