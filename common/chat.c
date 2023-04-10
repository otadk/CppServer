#include "head.h"

int add_to_reactor(int epollfd, int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        return -1;
    }
    return 0;
}

void* sub_reactor(void* arg) {
    int maxEvents = 5;

    int epollfd = *(int*)arg;
    struct epoll_event ev, events[maxEvents];
    for (;;) {
        int nfds = epoll_wait(epollfd, events, maxEvents, -1);
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            struct chat_msg msg;
            int ret;
            if ((ret = recv(fd, (void*)&msg, sizeof(msg), 0)) <= 0) {
                epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
            }
            if (sizeof(msg) != ret) {
                DBG(RED"<MsgErr> size err\n"NONE);
                continue;
            }
            DBG(YELLOW"<Recv> msg %s\n"NONE, msg.msg);
        }
    }
}