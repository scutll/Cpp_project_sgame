#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QPushButton>
#include <QMainWindow>
#include "../../src/common.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();
    void generate_map();
    void swap_point(point_ p1,point_ p2);




private slots:
    void setPoint(QPushButton* btn);

private:
    void swap_point(QPushButton* btn1,QPushButton* btn2);


    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
