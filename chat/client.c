#include "../common/head.h"

int main() {

    char ip[20] = "47.115.202.87";
    int port = 7788;

    char name[20] = "ChatClient";
    char password[20] = "123456";
    int sex = 0; // 0 for male, 1 for female

    printf(YELLOW"sex-->\n");
    scanf("%d", &sex);

    DBG(GREEN"<Init>\nip::%s\nname::%s\npassword::%s\nport::%d\nsex::%d\n"NONE, name, ip, password, port, sex);

    int sockfd;
    if ((sockfd = socket_connect(ip, port)) < 0) {
        perror("socket_connect");
        exit(1);
    }

    for (;;) {
        int mode;
        printf(YELLOW"mode-->(-1退出，0注册，1登陆，2发消息)\n"NONE);
        scanf("%d", &mode);

        struct chat_msg msg;
        strcpy(msg.msg, "");
        strcpy(msg.name, name);
        strcpy(msg.passwd, password);
        msg.sex = sex;

        if (mode == -1) {
            msg.type = CHAT_SIGNOUT;
        } else if (mode == 0) {
            msg.type = CHAT_SIGNUP;
        } else if (mode == 1) {
            msg.type = CHAT_SIGNIN;
        } else if (mode == 2) {
            scanf("%s", msg.msg);
            msg.type = CHAT_SENDMSG;
        } else {
            msg.type = CHAT_SIGNOUT;
        }

        send(sockfd, (void*)&msg, sizeof(msg), 0);

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        if (select(sockfd + 1, &rfds, NULL, NULL, &tv) <= 0) {
            fprintf(stderr, RED"<System> 101 server is out of service.\n"NONE);
            exit(1);
        }
        if (recv(sockfd, (void*)&msg, sizeof(struct chat_msg), 0) <= 0) {
            fprintf(stderr, RED"<System> 102 server is out of service.\n"NONE);
            exit(1);
        }
        if (msg.type & CHAT_ACK) {
            DBG(GREEN"<Recv> ACK "BLUE"%s\n"NONE, msg.msg);
        } else {
            DBG(RED"<Recv> NAK "BLUE"null\n"NONE);
        }
        
        if (mode == -1) {
            break;
        }
    }



    return 0;
}