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

private:
    Ui::AskLogin *ui;
};

#endif // ASKLOGIN_H
