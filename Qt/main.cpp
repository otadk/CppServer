#include "head.h"

int main(int argc, char *argv[]) {
    // QT启动
    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Qt_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    int port = 7788; // 服务器端口
    char ip[20] = "47.115.202.87"; // 服务器ip
    int sockfd = socketConnect(ip, port);

    Widget w(sockfd);
    w.show();

    std::thread t(recvMsg, &w); // 创建线程监听服务器发送消息
    return a.exec();
}

// 创建网络连接套接字
int socketConnect(const char* ip, int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        return -1;
    }
    return sockfd;
}

// 消息接受处理
void recvMsg(Widget* w) {
    Msg msg;
    for (;;) {
        recv(w->getSockfd(), (void*)&msg, sizeof(Msg), 0);
        w->getArea()->append(msg.msg);
    }
}
