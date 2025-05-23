#include "asklogin.h"
#include "ui_asklogin.h"
#include <QDialog>

AskLogin::AskLogin(QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::AskLogin)
{
    ui->setupUi(this);

    connect(ui->LoginRequestBtn,&QPushButton::clicked,this,&QDialog::accept);
    connect(this,&QDialog::close,this,&QDialog::reject);
}

AskLogin::~AskLogin()
{
    delete ui;
}

qint64 AskLogin::getUserAccount(){
    return this->ui->GetUserAccountLine->text().toLongLong();
}

QString AskLogin::getUserPassword(){
    return this->ui->GetUserPasswordLine->text();
}

void AskLogin::dealNoticeRefusedLogin(const qint64 userAccount) {
    ui->refuseLogin->setText("账号或密码错误，请检查您的账号密码");
    ui->refuseLogin->setStyleSheet("color: red;");
    ui->GetUserAccountLine->setText(QString::number(userAccount));
}



void AskLogin::on_RegisterRequestLogin_clicked()
{
    this->close();
    emit this->NoticeStartWindow_Register();
}

void AskLogin::AskLoginAfterRegisterAccepted(const qint64 userAccout,const QString& userPassword) {
    ui->GetUserAccountLine->setText(QString::number(userAccout));
    ui->GetUserPasswordLine->setText(userPassword);
}
