#ifndef ASKLOGIN_H
#define ASKLOGIN_H

#include <QDialog>

namespace Ui {
class AskLogin;
}

class AskLogin : public QDialog
{
    Q_OBJECT

public:
    explicit AskLogin(QDialog *parent = nullptr);
    ~AskLogin();
    qint64 getUserAccount();
    QString getUserPassword();
    void dealNoticeRefusedLogin(const qint64 userAccount);
    void AskLoginAfterRegisterAccepted(const qint64 userAccout,const QString& userPassword);

private slots:
    void on_RegisterRequestLogin_clicked();

    void on_LoginRequestBtn_clicked();

private:
    Ui::AskLogin *ui;

signals:
    void NoticeStartWindow_Register();
};

#endif // ASKLOGIN_H
