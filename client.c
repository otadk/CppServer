#include "client.h"

int main() {

    int port = 7788; // 服务器端口
    
    char ip[20] = "47.115.202.87"; // 服务器ip

    // 网络套接字连接
    int sockfd = socketConnect(ip, port);

    fd_set rfds;

    char name[20];
    
    for (;;) {

        printf(YELLOW"type-->(0登陆，1登出，2发消息)\n"NONE);
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        FD_SET(0, &rfds);

        select(sockfd + 1, &rfds, NULL, NULL, NULL);

        if (FD_ISSET(0, &rfds)) {
            // 终端输入
            char terminalIn[512];
            memset(terminalIn, 0, sizeof(terminalIn));
            read(0, terminalIn, sizeof(terminalIn));
            Msg msg;
            msg.type = -1;
            if (terminalIn[0] == '0') {
                scanf("%s", name);
                strcpy(msg.msg, name);
                msg.type = 0;
            } else if (terminalIn[0] == '1') {
                strcpy(msg.msg, name);
                msg.type = 1;
            } else if (terminalIn[0] == '2') {
                scanf("%s", msg.msg);
                msg.type = 2;
            }

            send(sockfd, (void*)&msg, sizeof(Msg), 0);

            if (msg.type == 1) {
                break;
            }

        } else if (FD_ISSET(sockfd, &rfds)) {
            // 收到消息
            Msg msg;
            recv(sockfd, (void*)&msg, sizeof(Msg), 0);
            if (recv <= 0) {
                break;
            }
            if (msg.type == 3) {
                printf(BLUE"<Success>"GREEN"%s\n"NONE, msg.msg);
            } else if (msg.type == 4) {
                printf(RED"<Unsuccess>"GREEN"%s\n"NONE, msg.msg);
            } else if (msg.type == 5) {
                printf(PINK"<Msg>"GREEN"%s\n"NONE, msg.msg);
            }
        }
    }

    return 0;
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