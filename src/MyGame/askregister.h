#ifndef ASKREGISTER_H
#define ASKREGISTER_H

#include <QDialog>

namespace Ui {
class AskRegister;
}

class AskRegister : public QDialog
{
    Q_OBJECT

public:
    explicit AskRegister(QDialog *parent = nullptr);
    ~AskRegister();
    void dealRefuseRegister();
    QString getUserName();
    QString getUserPsw();
    qint64 getUserAccount();


private slots:
    void on_RegisterBtn_clicked();

private:
    Ui::AskRegister *ui;
};

#endif // ASKREGISTER_H
