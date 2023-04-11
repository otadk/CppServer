#include "head.h"

Widget::Widget(int sockfd, QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    QObject::connect(ui->pushButton, &QPushButton::released, this, &Widget::LoginEvent);
    QObject::connect(ui->pushButton_2, &QPushButton::released, this, &Widget::LogoutEvent);
    QObject::connect(ui->pushButton_3, &QPushButton::released, this, &Widget::SendEvent);
    this->sockfd = sockfd;
    this->Area = ui->textBrowser;
}

Widget::~Widget() {
    delete ui;
}

void Widget::LoginEvent() {
    Msg msg;
    msg.type = 0;
    name = this->ui->lineEdit->text();
    strcpy(msg.msg, name.toStdString().c_str());
    send(sockfd, (void*)&msg, sizeof(Msg), 0);
}

void Widget::LogoutEvent() {
    Msg msg;
    msg.type = 1;
    strcpy(msg.msg, name.toStdString().c_str());
    send(sockfd, (void*)&msg, sizeof(Msg), 0);
}

void Widget::SendEvent() {
    Msg msg;
    msg.type = 2;
    strcpy(msg.msg, ui->textEdit->toPlainText().toStdString().c_str());
    send(sockfd, (void*)&msg, sizeof(Msg), 0);
    ui->textBrowser->append(name + "::" + msg.msg);
}

int Widget::getSockfd() {
    return sockfd;
}

QTextBrowser* Widget::getArea() {
    return Area;
}
