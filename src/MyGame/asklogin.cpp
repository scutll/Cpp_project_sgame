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
