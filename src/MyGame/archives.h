#ifndef ARCHIVES_H
#define ARCHIVES_H

#include <qcoreapplication.h>
#include <vector>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include "../../src/TerminalVersion/save_package.h"

namespace Ui {
class Archives;
}

class Archives : public QDialog
{
    Q_OBJECT

public:
    explicit Archives(QWidget *parent = nullptr);
    ~Archives();
    void flash();
    void toSave(){to_save = true;}
    void toLoad(){to_save = false;}

signals:
    void loadFinished();    //设置载入文件完成后的信号

private:
    void init();
    Ui::Archives *ui;

private slots:
    void Archives_name_clicked(QPushButton*,QLabel*);

private:
    std::vector<QPushButton*> archives;
    std::vector<QLabel*> time_labels;

    bool to_save;       //标记该窗口是用于保存游戏还是载入游戏，点击按钮时候要判断
    int archives_cnt;

    package pkg_forload;
    //存档保存地址
    QString path = QCoreApplication::applicationDirPath() + "/archives/";
};


#endif // ARCHIVES_H
