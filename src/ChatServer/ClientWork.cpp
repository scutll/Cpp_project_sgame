//
// Created by mxl_scut on 25-5-15.
//

#include "ClientWork.h"

#include <bits/fs_fwd.h>

ClientWork::ClientWork(const quintptr &handle, QObject *parent) :QObject(parent){
    this->socketDescriptor = handle; //获取套接字描述符
}

ClientWork::~ClientWork() {
    qDebug() << __TIME__ << " 客户端销毁: "<<QThread::currentThreadId();

}

void ClientWork::initializeSocket() {
    qDebug() << __TIME__ << " 客户端线程启动：" << QThread::currentThreadId();
    this->socket_id = QString::number(quintptr(QThread::currentThreadId())).toInt();

    qDebug() << "socket id: " << this->socket_id;
    this->socket = new QTcpSocket(this);
    this->socket->setSocketDescriptor(this->socketDescriptor);
    qDebug() << this->socket->peerAddress() << this->socket->peerPort();

    //接收消息
    connect(this->socket,&QTcpSocket::readyRead,this,&ClientWork::ReadData, Qt::DirectConnection);

    //socket断联
    connect(this->socket,&QTcpSocket::disconnected,this,&ClientWork::dealClientDisconnected, Qt::DirectConnection);
}


void ClientWork::dealClientDisconnected() {
    emit NoticeClientDisconnected(this->userName);  //通知其他用户该用户离线
    emit updateLocalUserData(this->userName,"0");
    emit clientDisconnected(socket_id);         //通知服务器用户离线
}

int ClientWork::SocketId() {
    return this->socket_id;
}



void ClientWork::dealNoticeClientDisconnected(const QString &userName) {
    QMutexLocker locker(&mutex);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << quint16(0);
    out << qint16(MSGTYPE::SendUserDisconnected);
    out << userName;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    this->socket->write(block);

    // emit updateLocalUserData(userName,"0");
}



void ClientWork::noticeClientLogin(const qint64 userAccount,const QString &userName) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);


    qDebug() << "通知 " << this->userAccount << " :" << userAccount << "已登录";
    out << quint16(0);
    out << qint16(MSGTYPE::NoticeNewLogin);
    out << userAccount;
    out << userName;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    socket->write(block);


    if(this->userAccount == userAccount){
        this->userName = userName;
    }
}

void ClientWork::noticeRefusedLogin(const qint64 userAccount) {
    if (this->userAccount != userAccount)
        return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << quint16(0);
    out << qint16(MSGTYPE::RefuseLogin);
    out << userAccount;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    socket->write(block);
}


void ClientWork::noticeRegisterAccepted(const qint64 userAccount,const QString& userName,const QString& extName) {
    if (this->register_Account != userAccount)
        return;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << quint16(0);
    out << qint16(MSGTYPE::RegisterAccepted);
    out << userAccount;
    out << userName;
    out << extName;

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    socket->write(block);
}

void ClientWork::noticeAccountOccupied(const qint64 userAccount) {
    if (register_Account != userAccount)
        return;
    qDebug() << userAccount << " register request :账号重复,无法注册";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << quint16(0);
    out << qint16(MSGTYPE::AccountOccupied);
    out << userAccount;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    socket->write(block);
}



void ClientWork::noticeRejectRepeatedName(const qint64 userAccount) {
    if(this->userAccount != userAccount)
        return;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << quint16(0);
    out << qint16(MSGTYPE::RepeatedNameRejected);
    out << userAccount;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    socket->write(block);
}

void ClientWork::noticeUserNameModified(const qint64 userAccount, const QString &userName, const QString &newName) {
    if (this->userAccount == userAccount) {
        this->userName = newName;
    }
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << quint16(0);
    out << qint16(MSGTYPE::UserNameModified);
    out << userAccount;
    out << userName;
    out << newName;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    socket->write(block);
}



void ClientWork::sendUserMessageToReceiver(const QString &senderName, const QString& receiverName, const QString &message) {
    if (this->userName != receiverName && receiverName != QString("All"))
        return;
    QMetaObject::invokeMethod(this,[this,senderName,message]() {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_8);

        out << quint16(0);
        out << qint16(MSGTYPE::SendNormalMesssage);
        out << senderName << message;

        out.device()->seek(0);
        out << quint16(block.size() - sizeof(quint16));

        this->socket->write(block);


    },Qt::QueuedConnection);

}



void ClientWork::ReadData() {
    qDebug() << __TIME__ << "message received";

    QDataStream in(this->socket);
    in.setVersion(QDataStream::Qt_6_8);

    forever{
        if (nextBlockSize == 0) {
            if (this->socket->bytesAvailable() < sizeof(quint16))
                break;
            in>>nextBlockSize;
        }
        if (nextBlockSize == 0xffff) {
            nextBlockSize = 0;
            break;
        }
        if (socket->bytesAvailable() < nextBlockSize) {
            break;
        }

        in >> MSG_TYPE;
        qDebug() << MSG_TYPE;

        if (MSG_TYPE == LoginRequest) {
            QString userPassword;
            qint64 userAccount;
            in >> userAccount;
            in >> userPassword;

            qDebug() << "login request: " << userAccount << userPassword;
            if(this->userAccount == 0){
                qDebug() << "线程处理登录: 修改账号为: " << userAccount;
                this->userAccount = userAccount;
            }
            //提交到server进行处理
            emit newClientLogin(userAccount,userPassword);
        }
        else if (MSG_TYPE == FriendApplication) {
            //暂时不开放
        }
        else if (MSG_TYPE == SendAcceptApplicationNotice) {
            //暂时不开放
        }
        else if (MSG_TYPE == NormalMessage) {
            QString sender;
            QString receiver;
            QString message;

            in >> sender >> receiver >> message;
            qDebug() << "message from " << sender << " to " << receiver << ": " << message;
            emit acceptUserNormalMessage(sender,receiver,message);
        }
        else if (MSG_TYPE == ModifyNameRequest) {
            QString newName;
            qint64 userAccount;

            in >> userAccount >> newName;
            qDebug() << userAccount << ": modify name request: " << newName;
            emit ModifyUserNameRequest(userAccount,newName);
        }
        else if (MSG_TYPE == MSGTYPE::RegisterRequest) {
            QString userName;
            qint64 userAccount;
            QString userPassword;

            in >> userAccount >> userPassword >> userName;
            register_Account = userAccount;
            qDebug() << "register request: " << userAccount << userPassword << userName;
            emit this->noticeRegisterRequest(userAccount,userPassword,userName);
        }

        nextBlockSize = 0;
        MSG_TYPE = -1;
    }

}
