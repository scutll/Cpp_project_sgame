#include "start_window.h"
#include "ui_start_window.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QListView>
#include <QToolButton>
#include <QLineEdit>
#include <QInputDialog>
#include <QDir>
#include "global.h"
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


    this->playerclient = Q_NULLPTR;
    this->chatclient = Q_NULLPTR;
}

start_window::~start_window()
{
    delete ui;
}

void start_window::on_offline_game_clicked()
{

    w->OfflineMode();
    w->show();
    w->generate_map();
}


void start_window::on_online_game_clicked()
{
    if (GLOB_IsConnectedToGameServer) {
        qDebug() << "正在发送匹配请求: ";
        w->OnlineMode();
        playerclient->INTERFACE_SendMatchRequest(GLOB_UserAccount);
        app_msg("游戏服务器", "正在匹配中......");
        ui->online_game->setText("匹配中");
        return;
    }


    if(!GLOB_IsConnectedToServer){
        qDebug() << "未登录";
        app_msg("系统","用户未登录，无法进行在线游戏");
        return;
    }
    if (matching)
        return;

    matching = true;
    app_msg("game服务器","正在连接游戏服务器......");

    playerclient = new PlayerClient(GLOB_UserAccount,this);
    playerclient->INTERFACE_SendLoginRequest(GLOB_UserAccount);

    if(!this->w){
        w = new MainWindow;
        w->init();


        connect(w,SIGNAL(GameClosed()),this,SLOT(gameClosed()));
    }

    connect(this->w,&MainWindow::app_msg_Signal,this,&start_window::app_msg);
    connect(this->w,&MainWindow::noticeGameStarted,this,&start_window::OnlineGameStart,Qt::QueuedConnection);
    connect(this->playerclient,&PlayerClient::INTERFACE_WaitingForMatch,this,&start_window::dealWaitingForMatch,Qt::QueuedConnection);
    connect(this->playerclient,&PlayerClient::INTERFACE_ServerDisconnected,this,&start_window::dealGameServerDisconnected,Qt::QueuedConnection);
    connect(this->playerclient,&PlayerClient::INTERFACE_LoginAccepted,w,&MainWindow::GameLoginSuccess,Qt::QueuedConnection);
    connect(this->playerclient,&PlayerClient::INTERFACE_StartGame,w,&MainWindow::gameStart,Qt::QueuedConnection);
    connect(this->playerclient,&PlayerClient::INTERFACE_LoseGame,w,&MainWindow::gameLose,Qt::QueuedConnection);
    connect(this->playerclient,&PlayerClient::INTERFACE_WinForQuit,w,&MainWindow::gameWinQuit,Qt::QueuedConnection);
    connect(this->playerclient,&PlayerClient::INTERFACE_WinGame,w,&MainWindow::gameWin,Qt::QueuedConnection);
    connect(this->w,&MainWindow::GameEnded,this,&start_window::dealGameEnded,Qt::QueuedConnection);
    connect(this->w,&MainWindow::finish_online,this->playerclient,&PlayerClient::INTERFACE_userFinished,Qt::QueuedConnection);


    ui->online_game->setText("开始匹配");

}

void start_window::dealGameServerDisconnected(){
    app_msg("系统", "游戏服务器已经断开", true);

    this->w->close();

    //删除游戏线程
    playerclient->deletePlayerThread();
    playerclient->deleteLater();

}

void start_window::dealWaitingForMatch(){
    app_msg("游戏服务器", "正在寻找对手......");
}

void start_window::dealGameEnded(bool win) {
    matching = false;
    ui->online_game->setText("开始匹配");
    this->w->close();
    this->show();

    if (win) {
        app_msg("游戏服务器", "游戏结束，胜利！");
    }
    else {
        app_msg("游戏服务器" , "游戏结束，失败！");
    }
}


void start_window::dealGameClosed() {
    app_msg("系统", "您已退出, 游戏结束");
    playerclient->INTERFACE_userQuited(GLOB_UserAccount);
}


void start_window::on_Awake_signal(){
    w->close();
    qDebug()<<"close game window";
    this->show();
    qDebug()<<"awake start window";
}

void start_window::app_msg(const QString& sender,const QString& message,bool error){
    QString messageLine = "[" + sender + "]  " + message;


    QStandardItem *item = new QStandardItem(messageLine);
    if(error)
        item->setForeground(QBrush(QColor("red")));
    model->appendRow(item);


    QModelIndex lastIndex = model->index(model->rowCount() - 1, 0);
    lastIndex = model->index(model->rowCount() - 1, 0);
    ui->msg_list_window->scrollTo(lastIndex);
}

void start_window::gameClosed(){
    emit playerclient->INTERFACE_userQuited(GLOB_UserAccount);
    this->show();
}

void start_window::OnlineGameStart() {
    // this->close();
    w->OnlineMode();
    w->show();
}

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

            qDebug() << (void*)chatclient;


            if(chatclient != nullptr)
                delete chatclient;   //把这个删掉就不会崩溃了????????????????????????????????????????????????

            qDebug() << "获取用户输入: " << userAccount << userPassword;
            chatclient = new Client(QString::number(userAccount),userAccount);
            qDebug() << (void*)chatclient;
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
    this->userName = userName;

    ui->LoginBtn->setText("已登录: (" + QString::number(userAccount) + ") " + userName);
    QString Title = QString(QString::number(GLOB_UserAccount) + "  " + GLOB_UserName);
    this->setWindowTitle(Title);

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
    connect(this->w,&MainWindow::close,this,&start_window::dealGameClosed,Qt::QueuedConnection);
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
