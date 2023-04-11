#ifndef HEAD_H
#define HEAD_H
#include "ui_widget.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QWidget>
#include <QTextBrowser>
#include <thread>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT
public:
    Widget(int sockfd, QWidget *parent = nullptr);
    ~Widget();
    int getSockfd();
    QTextBrowser* getArea();

private:
    Ui::Widget *ui;
    QString name;
    void LoginEvent();
    void LogoutEvent();
    void SendEvent();
    int sockfd;
    QTextBrowser* Area;
};

typedef struct Message {
    int type;
    char msg[512];
} Msg;

int socketConnect(const char* ip, int port);
void recvMsg(Widget* w);


#endif // HEAD_H
