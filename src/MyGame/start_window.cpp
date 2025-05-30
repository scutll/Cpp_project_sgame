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
#include "askregister.h"
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
    app_msg("game服务器","正在连接游戏服务器......");

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
    QString messageLine = message;

    QStandardItem *_Line = new QStandardItem(QString("[" + sender + "] "));
    model->appendRow(_Line);

    QModelIndex lastIndex = model->index(model->rowCount() - 1, 0);
    ui->msg_list_window->scrollTo(lastIndex);

    QStandardItem *item = new QStandardItem(messageLine);
    if(error)
        item->setForeground(QBrush(QColor("red")));
    model->appendRow(item);


    lastIndex = model->index(model->rowCount() - 1, 0);
    ui->msg_list_window->scrollTo(lastIndex);
}

void start_window::onConnectionSucceeded() {
    qDebug() << "Connection succeeded!";
    app_msg("game服务器", "连接服务器成功，正在匹配：");
    GameServer->send_match_request();
    // GameServer->send_msg("hello");
}

void start_window::onConnectionFailed(const QString &errorMessage) {
    qDebug() << "Connection failed:" << errorMessage;
    app_msg("game服务器", errorMessage, true);
    matching = false;
}

void start_window::onRecv_msg_str(const QString& msg){
    app_msg("game服务器",msg);
}

void start_window::onRecv_msg_package(const package& pkg){
    app_msg("game服务器","匹配成功,正在进入游戏");

    for(int i=0;i<36;i++){
        app_msg("game服务器", QString::number(pkg.map[i].value));
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


void start_window::dealAskRegister(){
    qint64 userAccount;
    QString userPassword;
    QString userName;
    AskRegister* registerDialog = new AskRegister(this);
    qDebug() << "正在打开注册界面";
    if(registerDialog->exec() == QDialog::Accepted){
        userAccount = registerDialog->getUserAccount();
        userPassword = registerDialog->getUserPsw();
        userName = registerDialog->getUserName();

        this->registeredPassword = userPassword;

        if(chatclient)
            delete chatclient;

        chatclient = new Client(userName,userAccount + 0xff);
        this->init_chatclient();
        clientConnected = true;
        qDebug() << "注册: " << userAccount << userPassword << userName;
        chatclient->INTERFACE_registerRequest(userAccount,userPassword,userName);
    }

}


void start_window::on_LoginBtn_clicked()
{
    if(!userLoged){
        qint64 userAccount;
        QString userPassword;
        AskLogin* loginDialog = new AskLogin(this);
        connect(loginDialog,&AskLogin::NoticeStartWindow_Register,this,&start_window::dealAskRegister);
        qDebug() << "正在打开登录界面";
        if(loginDialog->exec() == QDialog::Accepted){
            userAccount = loginDialog->getUserAccount();
            userPassword = loginDialog->getUserPassword();

            this->temp_password = userPassword;

            if(chatclient)
                delete chatclient;

            chatclient = new Client(QString::number(userAccount),userAccount);
            this->init_chatclient();
            clientConnected = true;
            chatclient->INTERFACE_LoginRequest(userAccount,userPassword);
        }
        else{
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


void start_window::dealINTERFACE_dealAcceptNormalMessage(const QString& senderName,const QString& message, const QString& sendTime){
    app_msg(senderName + "--" + sendTime, message);
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
    // ui->LoginBtn->repaint();  // 强制立即重绘，不经过事件队列

    app_msg("系统","登录成功: " + userName,false);

}

void start_window::dealINTERFACE_dealRefusedLogin(const qint64 userAccount) {
    this->userLoged = false;
    GLOB_IsConnectedToServer = true;
    GLOB_UserAccount = userAccount;
    qint64 newAccount;

    // app_msg("系统","登录失败,密码或账号错误" + userName,true);
    AskLogin* loginDialog = new AskLogin(this);
    loginDialog->dealNoticeRefusedLogin(userAccount);
    qDebug() << "(密码错误)正在再次打开登录界面";
    QString newPassword;
    if(loginDialog->exec() == QDialog::Accepted){
        newAccount = loginDialog->getUserAccount();
        newPassword = loginDialog->getUserPassword();

        this->temp_password = newPassword;
        GLOB_UserAccount = newAccount;
        chatclient->INTERFACE_LoginRequest(GLOB_UserAccount,newPassword);
    }

}

void start_window::dealINTERFACE_accountAlreadyLogined(const qint64 userAccount) {
    if(userLoged)
        return;

    this->userLoged = false;
    GLOB_IsConnectedToServer = true;
    GLOB_UserAccount = userAccount;
    qint64 newAccount;

    //账号已经被登录
    AskLogin* loginDialog = new AskLogin(this);
    loginDialog->dealNoticeAccountAlreadyLogined(userAccount);
    qDebug() << "(账号已登录)正在再次打开登录界面";
    QString newPassword;
    if(loginDialog->exec() == QDialog::Accepted){
        newAccount = loginDialog->getUserAccount();
        newPassword = loginDialog->getUserPassword();

        this->temp_password = newPassword;
        GLOB_UserAccount = newAccount;
        chatclient->INTERFACE_LoginRequest(GLOB_UserAccount,newPassword);
    }

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

void start_window::dealINTERFACE_noticeAccountOccupied(const qint64 userAccount){
    qint64 user_Account;
    QString userPassword;
    QString userName;
    AskRegister* registerDialog = new AskRegister(this);
    registerDialog->dealRefuseRegister();
    qDebug() << "正在重新打开注册界面";
    if(registerDialog->exec() == QDialog::Accepted){
        user_Account = registerDialog->getUserAccount();
        userPassword = registerDialog->getUserPsw();
        userName = registerDialog->getUserName();
        this->registeredPassword = userPassword;

        if(chatclient)
            delete chatclient;

        chatclient = new Client(userName,userAccount + 0xff);
        this->init_chatclient();
        clientConnected = true;
        qDebug() << "注册: " << userAccount << userPassword << userName;
        chatclient->INTERFACE_registerRequest(user_Account,userPassword,userName);
    }
}

void start_window::dealINTERFACE_noticeRegisterAccepted(const qint64 userAccount, const QString& userName,const QString& extName){
    QString extraStr;
    if(userName != extName){
        extraStr = userName + "已经被占用, 将使用默认初始名称: " + extName;
    }


    //注册成功后要先删除原来的临时客户端线程，否则会导致提醒两次登录
    chatclient->deleteChatThread();

    if (!userLoged) {
        qint64 account;
        QString password;
        AskLogin* loginDialog = new AskLogin(this);
        loginDialog->AskLoginAfterRegisterAccepted(userAccount,this->registeredPassword);
        connect(loginDialog,&AskLogin::NoticeStartWindow_Register,this,&start_window::dealAskRegister);
        qDebug() << "允许登录,正在打开登录界面";
        if(loginDialog->exec() == QDialog::Accepted){
            account = loginDialog->getUserAccount();
            password = loginDialog->getUserPassword();

            this->temp_password = password;

            if(chatclient)
                delete chatclient;

            chatclient = new Client(QString::number(account),account);
            this->init_chatclient();
            clientConnected = true;
            chatclient->INTERFACE_LoginRequest(account,password);
        }
        else{
            return;
        }
    }
}



void start_window::init_chatclient(){
    connect(chatclient,&Client::INTERFACE_dealUserDisconnected,this,&start_window::dealINTERFACE_dealUserDisconnected);
    connect(chatclient,&Client::INTERFACE_dealUserLogined,this,&start_window::dealINTERFACE_dealUserLogined);
    connect(chatclient,&Client::INTERFACE_dealAcceptNormalMessage,this,&start_window::dealINTERFACE_dealAcceptNormalMessage);
    connect(chatclient,&Client::INTERFACE_dealConnnectError,this,&start_window::dealINTERFACE_dealConnnectError);
    connect(chatclient,&Client::INTERFACE_ServerDisconnected,this,&start_window::dealINTERFACE_dealServerDisconnected);
    connect(chatclient,&Client::INTERFACE_RefusedLogin,this,&start_window::dealINTERFACE_dealRefusedLogin);
    connect(chatclient,&Client::INTERFACE_AccountAlreadyLogined,this,&start_window::dealINTERFACE_accountAlreadyLogined);
    connect(chatclient,&Client::INTERFACE_LoginAccepted,this,&start_window::dealINTERFACE_dealLoginAccepted);
    connect(chatclient,&Client::INTERFACE_NameModifyAccepted,this,&start_window::dealINTERFACE_dealNameModifyAccepted);
    connect(chatclient,&Client::INTERFACE_NoticeUserNameModified,this,&start_window::dealINTERFACE_dealNoticeUserNameModified);
    connect(chatclient,&Client::INTERFACE_repeatedName,this,&start_window::dealINTERFACE_repeatedName);
    connect(chatclient,&Client::INTERFACE_NoticeAccountOccupied,this,&start_window::dealINTERFACE_noticeAccountOccupied);
    connect(chatclient,&Client::INTERFACE_NoticeRegisterAccepted,this,&start_window::dealINTERFACE_noticeRegisterAccepted);
}
