//
// Created by mxl_scut on 25-5-15.
//
//VERSION 1.0 服务器不做本地记录，玩家直接设置账号和用户名，直接指定消息发送给谁或者全体（All）
#include "ChatServer.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>

ChatServer::ChatServer(QObject *parent)
    :QObject(parent){
    this->tcpServer = new TcpServer(this);
    connect(tcpServer, &TcpServer::newClientConnected, this,&ChatServer::newClientConnection,Qt::DirectConnection);


    UserInfoPath = QCoreApplication::applicationDirPath() + "/UserInfo/";
    QDir infodir(QCoreApplication::applicationDirPath());
    if (!infodir.exists("UserInfo")) {
        qDebug() << "UserInfo文件夹不存在，正在初始化......";
        if (!infodir.mkdir("UserInfo"))
            qDebug() << "UserInfo 文件夹创建失败";
    }

    UserPasswordPath = QCoreApplication::applicationDirPath() + "/UserPassword/";
    if (!infodir.exists("UserPassword")) {
        qDebug() << "UserPassword文件夹不存在，正在初始化......";
        if (!infodir.mkdir("UserPassword"))
            qDebug() << "UserPassword 文件夹创建失败";
    }

    UserUnsentMessagePath = QCoreApplication::applicationDirPath() + "/UnsentMessage/";
    if (!infodir.exists("UnsentMessage")) {
        qDebug() << "UnsentMessage文件夹不存在，正在初始化......";
        if (!infodir.mkdir("UnsentMessage"))
            qDebug() << "UnsentMessage 文件夹创建失败";
    }

}

ChatServer::~ChatServer() {
    foreach(QThread* thread,this->clients.keys()) {
        this->clients.value(thread)->deleteLater();
        thread->quit();
        thread->wait();
        thread->deleteLater();
    }
    this->clients.clear();
}


void ChatServer::newClientConnection(const qintptr handle) {
    QThread* thread = new QThread;
    //请求连接的客户端的handle
    ClientWork* work = new ClientWork(handle);
    work->moveToThread(thread);
    this->clients.insert(thread,work);

    connect(thread,&QThread::started,work,&ClientWork::initializeSocket,Qt::DirectConnection);
    thread->start();

    //用户注册请求
    connect(work,&ClientWork::noticeRegisterRequest,this,&ChatServer::dealRegisterRequest,Qt::QueuedConnection);
    connect(this,&ChatServer::transferAccountOccupied,work,&ClientWork::noticeAccountOccupied,Qt::QueuedConnection);
    connect(this,&ChatServer::transferRegisterAccepted,work,&ClientWork::noticeRegisterAccepted,Qt::QueuedConnection);


    //用户修改名称
    connect(work,&ClientWork::ModifyUserNameRequest,this,&ChatServer::dealModifyName,Qt::QueuedConnection);
    connect(this,&ChatServer::transferRejectRepeatedName,work,&ClientWork::noticeRejectRepeatedName,Qt::QueuedConnection);
    connect(this,&ChatServer::transferUserNameModified,work,&ClientWork::noticeUserNameModified,Qt::QueuedConnection);

    //用户断联的处理
    connect(work,&ClientWork::NoticeClientDisconnected,this,&ChatServer::dealNoticeClientDisconnected,Qt::QueuedConnection);
    connect(this,&ChatServer::transferNoticeDisconnected,work,&ClientWork::dealNoticeClientDisconnected,Qt::QueuedConnection);
    connect(work,&ClientWork::updateLocalUserData,this,&ChatServer::dealUpdateLocalUserData,Qt::QueuedConnection);

    //通知
    connect(work,&ClientWork::clientDisconnected,this,&ChatServer::dealClientDisconnected,Qt::QueuedConnection);

    //用户登录
    connect(work,&ClientWork::newClientLogin,this,&ChatServer::dealNewClientLogin,Qt::QueuedConnection);
    connect(this,&ChatServer::transferNewClientLogin,work,&ClientWork::noticeClientLogin,Qt::QueuedConnection);
    connect(this,&ChatServer::transferRefuseLogin,work,&ClientWork::noticeRefusedLogin,Qt::QueuedConnection);
    connect(this,&ChatServer::transferAccountAlreadyLogined,work,&ClientWork::noticeAccountAlreadyLogined,Qt::QueuedConnection);

    //接收普通消息
    connect(work,&ClientWork::acceptUserNormalMessage,this,&ChatServer::dealAcceptUserNormalMessage,Qt::QueuedConnection);
    connect(this,&ChatServer::transferAcceptUserNormalMessage,work,&ClientWork::sendUserMessageToReceiver,Qt::QueuedConnection);
}

void ChatServer::dealNoticeClientDisconnected(const QString &userName) {
    if(userName.isEmpty())
        return;
    emit this->transferNoticeDisconnected(userName);
}


void ChatServer::dealUpdateLocalUserData(const QString &userName, const QString &type) {
    if (type == "1") {
        qDebug() << userName << "added to users:";
        existingUsers.push_back(userName);
    }
    else if (type == "0") {
        qDebug() << userName << "removed from users:";
        for (int i=0;i < existingUsers.size();i++) {
            if (existingUsers.at(i) == userName) {
                existingUsers.erase(existingUsers.begin()+i);
                break;
            }
        }
    }

    qDebug() << "existingUsers: ";
    for (const QString& user: this->existingUsers) {
        qDebug() << user;
    }
}



void ChatServer::dealClientDisconnected(const int socket_id) {
    for (QThread* thread : this->clients.keys()) {
        if (this->clients.value(thread)->SocketId() == socket_id) {
            this->clients.value(thread)->deleteLater();

            thread->requestInterruption();
            thread->exit();
            thread->wait();
            this->clients.remove(thread);
            thread->deleteLater();
            break;
        }
    }

}

void ChatServer::dealRegisterRequest(const qint64 userAccount,const QString& userPassword,const QString& userName) {
    if (accountOccupied(userAccount)) {
        emit transferAccountOccupied(userAccount);
        return;
    }
    QString extName;
    if (nameExists(userName))
        extName = QString::number(userAccount);
    else
        extName = userName;

    this->SaveUserInfo(extName,userAccount);
    this->SaveUserPsw(userAccount,userPassword);
    emit this->transferRegisterAccepted(userAccount,userName,extName);

}


void ChatServer::dealModifyName(const qint64 userAccount, const QString &newName) {
    if (nameExists(newName)) {
        emit this->transferRejectRepeatedName(userAccount);
        return;
    }
    QString userName = getUserName(userAccount);
    updateUserName(userAccount,newName);
    emit this->transferUserNameModified(userAccount,userName,newName);
}




void ChatServer::dealNewClientLogin(const qint64 userAccount,const QString &userPassword) {
    qDebug() << userAccount << " requesting login ; password: " << userPassword;
    //检查是否可以登录
    int allowLogin = this->checkLoginInfo(userAccount,userPassword);
    if (allowLogin == 1) {
        qDebug() << userAccount << "login refused: Wrong password";
        emit transferRefuseLogin(userAccount);
        return;
    }
    else if (allowLogin == 2) {
        qDebug() << userAccount << "already logined";
        emit transferAccountAlreadyLogined(userAccount);
        return;
    }

    QString usrName = getUserName(userAccount);
    emit transferNewClientLogin(userAccount,usrName);
    sendUnsentMessage(userAccount,usrName);
    dealUpdateLocalUserData(usrName,"1");
}

int ChatServer::checkLoginInfo(const qint64 userAccount,const QString &userPassword) {
    qDebug() << "正在检查用户密码";
    qint64 Account;
    QString Password;
    QString filename = this->UserPasswordPath + "accpsw.psw";

    //加入注册功能，没有用户直接拒绝
    QFile userInfoFile(this->UserInfoPath + QString::number(userAccount) + ".usr");
    if(!userInfoFile.open(QIODevice::ReadOnly)){
        qDebug() << userAccount << "不存在";
        return 1;
    }

    //检查用户是否已经登录
    if (AccountLogined(userAccount)) {
        qDebug() << userAccount << "已经登录";
        return 2;
    }

    QFile usrAccountPasswordFile(filename);
    if (!usrAccountPasswordFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开密码映射文件 " + this->UserPasswordPath + "accpsw.psw" << " : " << usrAccountPasswordFile.errorString();
        return 1;
    }

    QDataStream in(&usrAccountPasswordFile);
    in.setVersion(QDataStream::Qt_6_8);
    while (!in.atEnd()) {
        in >> Account >> Password;
        if (Account != userAccount)
            continue;
        else{
            qDebug() << "找到用户 " << Account;
            if (userPassword != Password)
            {
                qDebug() << "用户密码错误 : " << userPassword << "正确密码：" <<Password;
                return 1;
            }
            else
                return 0;
        }
    }

    return 1;

}

bool ChatServer::AccountLogined(const qint64 userAccount) {
    foreach(QThread* thread,this->clients.keys()) {
        const qint64 account = this->clients.value(thread)->Account();
        bool logined = this->clients.value(thread)->Logined();
        if (account == userAccount && logined)
            return true;
    }
    return false;
}

bool ChatServer::nameExists(const QString &name) {
    QDir dir(this->UserInfoPath);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    foreach(const QFileInfo& fileinfo, fileList) {
        QString fileName = UserInfoPath + fileinfo.fileName();
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开用户信息文件 " + fileName << " : " << file.errorString();
            return "";
        }
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_6_8);

        qint64 account;
        QString userName;
        in >> account;
        in >> userName;
        if (name == userName)
            return true;
    }
    return false;
}

bool ChatServer::accountOccupied(const qint64 userAccount) {
    QDir dir(this->UserInfoPath);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    foreach(const QFileInfo& fileinfo, fileList) {
        QString fileName = UserInfoPath + fileinfo.fileName();
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开用户信息文件 " + fileName << " : " << file.errorString();
            return "";
        }
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_6_8);

        qint64 account;
        QString userName;
        in >> account;
        in >> userName;

        qDebug() << "找到了: " << account << userName;
        if (userAccount == account)
            return true;
    }
    return false;
}

void ChatServer::updateUserName(const qint64 userAccount,const QString &newName) {
    QString filename = this->UserInfoPath + QString::number(userAccount) + ".usr";
    QFile userfile(filename);
    if (!userfile.open(QIODevice::WriteOnly)) {
        qDebug() << "修改用户名---无法打开用户信息文件 " + filename << " : " << userfile.errorString();
        return;
    }
    QDataStream out(&userfile);
    out.setVersion(QDataStream::Qt_6_8);

    out.device()->seek(0);
    out << userAccount << newName;

    userfile.close();

}

QString ChatServer::getUserName(const qint64 userAccount) {
    QString filename = this->UserInfoPath + QString::number(userAccount) + ".usr";
    QFile userInfoFile(filename);

    if (!userInfoFile.open(QIODevice::ReadOnly)) {
        qDebug() << "查询用户名---无法打开用户信息文件 " + filename << " : " << userInfoFile.errorString();
        return "";
    }
    QDataStream in(&userInfoFile);
    in.setVersion(QDataStream::Qt_6_8);

    qint64 account;
    QString userName;
    in >> account;
    in >> userName;

    qDebug() << userAccount << " 用户名: " << userName;
    return userName;


}

qint64 ChatServer::getUserAccount(const QString &userName) {
    QDir dir(this->UserInfoPath);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    foreach(const QFileInfo& fileinfo, fileList) {
        QString fileName = UserInfoPath + fileinfo.fileName();
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开用户信息文件 " + fileName << " : " << file.errorString();
            return 0;
        }
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_6_8);

        qint64 account;
        QString Name;
        in >> account;
        in >> Name;

        if (userName == Name) {
            qDebug() << "找到了: " << account << Name;
            return account;

        }
    }
    return 0;
}

void ChatServer::saveUnsentMessage(const QString& senderName,const QString& receiverName,const QString& message) {
    QString filename = this->UserUnsentMessagePath + QString::number(getUserAccount(receiverName)) + ".usm";
    QFile userInfoFile(filename);

    if (!userInfoFile.open(QIODevice::Append)) {
        qDebug() << "保存未发送信息---无法打开用户信息文件 " + filename << " : " << userInfoFile.errorString();
        return;
    }
    QDataStream out(&userInfoFile);
    out.setVersion(QDataStream::Qt_6_8);

    out << senderName << message;

    qDebug() << "存入未发送消息: " << "from " << senderName << " to " << receiverName << " : " << message;

}

void ChatServer::sendUnsentMessage(const qint64 userAccount, const QString &userName) {
    qDebug() << "正在查询未发送信息 of: " << userAccount;
    QString filename = this->UserUnsentMessagePath + QString::number(userAccount) + ".usm";
    QFile unsentFile(filename);

    if (!unsentFile.open(QIODevice::ReadOnly)) {
        qDebug() << "查询未发送信息---无法打开用户信息文件 " + filename << " : " << unsentFile.errorString();
        return;
    }
    QString sender;
    QString receiver = userName;
    QString message;

    QDataStream in(&unsentFile);
    in.setVersion(QDataStream::Qt_6_8);
    while (!in.atEnd()) {
        in >> sender >> message;
        emit transferAcceptUserNormalMessage(sender,receiver,message);
    }
    unsentFile.close();

    QFile resizefile(filename);
    resizefile.open(QIODevice::WriteOnly);
    resizefile.close();
}




void ChatServer::dealAcceptUserNormalMessage(const QString &senderName, const QString &receiverName,
    const QString &message) {
    if (AccountLogined(getUserAccount(receiverName)))
        emit transferAcceptUserNormalMessage(senderName,receiverName,message);
    qDebug() << receiverName << "未登录，缓存到Unsent区";
    saveUnsentMessage(senderName,receiverName,message);
}


void ChatServer::SaveUserInfo(const QString& userName,const qint64 userAccount) {
    QString filename = this->UserInfoPath + QString::number(userAccount) + ".usr";
    QFile usrinfoFile(filename);

    if (!usrinfoFile.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开用户信息文件 " + filename << " : " << usrinfoFile.errorString();
        return;
    }
    QDataStream out(&usrinfoFile);
    out.setVersion(QDataStream::Qt_6_8);

    out << userAccount << userName;

    usrinfoFile.close();


}
void ChatServer::SaveUserPsw(const qint64 userAccount, const QString& password) {
    qDebug() << "保存用户账号 - 密码: " << userAccount << password;

    QString filename = this->UserPasswordPath + "accpsw.psw";
    QFile usrAccountPasswordFile(filename);
    if (!usrAccountPasswordFile.open(QIODevice::Append | QIODevice::ReadWrite)) {
        qDebug() << "无法打开密码映射文件 " + this->UserPasswordPath + "accpsw.psw" << " : " << usrAccountPasswordFile.errorString();
        return;
    }
    QDataStream out(&usrAccountPasswordFile);
    out.setVersion(QDataStream::Qt_6_8);

    out << userAccount << password;

    usrAccountPasswordFile.close();
}
