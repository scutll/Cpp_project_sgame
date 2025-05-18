#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),
    model(new QStandardItemModel(this))
{
    ui->messageListDisplay = new QListView;
    ui->setupUi(this);
    ui->messageListDisplay->setModel(model);

}

Widget::~Widget()
{
    delete ui;
}


void Widget::app_message(const QString& sender,const QString& message,bool error){
    QString line = "[" + sender + "] " + message;

    QStandardItem *item = new QStandardItem(line);
    if(error)
        item->setForeground(QBrush(QColor("red")));
    model->appendRow(item);

    QModelIndex lastIndex = model->index(model->rowCount() - 1, 0);
    ui->messageListDisplay->scrollTo(lastIndex);
}

//只需要调用Client提供的接口

void Widget::on_sendBtn_clicked()
{
    if(ui->TargetReceiver->text().size() == 0 || ui->MessageLine->text().size() == 0)
        return;
    client->INTERFACE_SendUserMessage(this->client->userName_(),ui->TargetReceiver->text(),ui->MessageLine->text());
    qDebug() << "from " << this->client->userName_() << " to " << ui->TargetReceiver->text() << ": " << ui->MessageLine->text();
}




void Widget::on_retrybtn_clicked()
{
    this->client->INTERFACE_retryConnect();
}

void Widget::init_client() {

    connect(this->client,&Client::INTERFACE_dealAcceptNormalMessage,this,&Widget::deal_INTERFACE_AcceptNormalMessage,Qt::QueuedConnection);
    connect(this->client,&Client::INTERFACE_dealUserLogined,this,&Widget::deal_INTERFACE_Logined,Qt::QueuedConnection);
    connect(this->client,&Client::INTERFACE_dealUserDisconnected,this,&Widget::deal_INTERFACE_Disconnected,Qt::QueuedConnection);
}


void Widget::deal_INTERFACE_AcceptNormalMessage(const QString &senderName, const QString &message) {
    this->app_message(senderName,message,false);
}

void Widget::deal_INTERFACE_Logined(const QString &userName) {
    QString message = userName + " 已上线";
    this->app_message("系统",message,false);
}

void Widget::deal_INTERFACE_Disconnected(const QString &userName) {
    QString messsage = userName + "已离线";
    this->app_message("系统",messsage,true);
}
