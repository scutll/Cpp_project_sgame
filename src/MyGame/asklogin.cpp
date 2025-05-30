#include "asklogin.h"
#include "ui_asklogin.h"
#include <QDialog>

AskLogin::AskLogin(QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::AskLogin)
{
    ui->setupUi(this);

    connect(ui->LoginRequestBtn,&QPushButton::clicked,this,&AskLogin::on_LoginRequestBtn_clicked);
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

void AskLogin::dealNoticeAccountAlreadyLogined(const qint64 userAccount){
    ui->refuseLogin->setText("账号已经被登录！");
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

void AskLogin::on_LoginRequestBtn_clicked()
{
    bool ok;
    ui->GetUserAccountLine->text().toLongLong(&ok);

    if(!ok){
        ui->refuseLogin->setText("账号格式错误: 请输入数字");
        ui->refuseLogin->setStyleSheet("color: red;");
        return;
    }

    accept();
}