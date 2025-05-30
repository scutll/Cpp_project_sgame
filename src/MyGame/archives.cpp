#include "archives.h"
#include "ui_archives.h"
#include <QDir>
#include <string>
#include <fstream>
#include <ctime>
#include <iostream>
#include "../../src/TerminalVersion/save_package.h"
#include <QDebug>
Archives::Archives(QWidget *parent)
    : QDialog(parent), ui(new Ui::Archives)
{
    ui->setupUi(this);
    init();

    for (int i = 0; i < 8; i++)
    {
        QPushButton *btn = archives[i];
        QLabel *label = time_labels[i];
        connect(btn, &QPushButton::clicked, btn, [this, btn, label]
                { Archives_name_clicked(btn, label); });
    }
}

Archives::~Archives()
{
    delete ui;
}

void Archives::flash()
{
    archives.clear();
    time_labels.clear();
    init();
}

void Archives::init()
{

    // 将button和label存入
    for (int i = 1; i <= 8; i++)
    {
        std::string obj_name = "game" + std::to_string(i);
        QPushButton *btn = findChild<QPushButton *>(obj_name);
        archives.push_back(btn);

        obj_name = "time_label" + std::to_string(i);
        QLabel *label = findChild<QLabel *>(obj_name);
        time_labels.push_back(label);
    }

    // 遍历文件夹，将文件填入button和label(文件数小于等于8)

    archives_cnt = 0;

    QDir dir(path);
    if (!dir.exists())
    {
        qDebug() << "文件夹不存在：" << dir.absolutePath();
        return;
    }
    QStringList files = dir.entryList(QDir::Files);
    qDebug() << "文件夹已打开";
    for (int i = 0; i < files.size(); i++)
    {
        QString filename = files.at(i);
        filename = (path + filename);
        std::ifstream fin(filename.toStdString(), std::ios::in | std::ios::binary);
        if (!fin.is_open())
        {
            qDebug() << "无法读取文件" << filename;
            continue;
        }

        package archive;
        fin.read(reinterpret_cast<char *>(&archive), sizeof(package));
        archives_cnt++;

        // 处理存档名和时间
        qDebug() << archive.archive_name;
        QString arc_name(archive.archive_name);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &archive.date);
        QString arc_time = QString::fromUtf8(buffer);

        // 设置文本
        archives[archives_cnt - 1]->setText(arc_name);
        time_labels[archives_cnt - 1]->setText(arc_time);
    }

    QString empty_game = "空存档";
    QString empty_time = "--------------------";
    for (int i = archives_cnt; i < 8; i++)
    {
        archives[i]->setText(empty_game);
        time_labels[i]->setText(empty_time);
    }


    qDebug() << "init finished";
}
