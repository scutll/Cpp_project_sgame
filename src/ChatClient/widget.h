#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include "Client.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    void deal_INTERFACE_AcceptNormalMessage(const QString& senderName,const QString& message);
    void deal_INTERFACE_Logined(const QString& userName);
    void deal_INTERFACE_Disconnected(const QString& userName);
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void app_message(const QString& sender,const QString& message,bool error);
    void init_client();

    Client* client;

private slots:
    void on_sendBtn_clicked();

    void on_retrybtn_clicked();

private:
    Ui::Widget *ui;
    QStandardItemModel *model;
};
#endif // WIDGET_H
