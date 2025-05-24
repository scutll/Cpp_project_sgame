#include "askregister.h"
#include "ui_askregister.h"

AskRegister::AskRegister(QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::AskRegister)
{
    ui->setupUi(this);

    connect(ui->RegisterBtn,&QPushButton::clicked,this,&AskRegister::on_RegisterBtn_clicked);
    connect(this,&QDialog::close,this,&QDialog::reject);
}

AskRegister::~AskRegister()
{
    delete ui;
}

QString AskRegister::getUserPsw(){
    return ui->UserPassword->text();
}

QString AskRegister::getUserName(){
    return ui->UserName->text();
}

qint64 AskRegister::getUserAccount(){
    return ui->UserAccount->text().toLongLong();
}

void AskRegister::dealRefuseRegister(){
    ui->RefuseRegister->setText("账号已被注册，请使用其他账号");
    ui->RefuseRegister->setStyleSheet("color: red;");
}


void AskRegister::on_RegisterBtn_clicked()
{    bool ok;
    ui->UserAccount->text().toLongLong(&ok);

    if(!ok){
        ui->RefuseRegister->setText("账号格式错误: 请输入数字");
        ui->RefuseRegister->setStyleSheet("color: red;");
        return;
    }
    accept();
}

