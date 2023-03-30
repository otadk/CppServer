#include "common/head.h"

int clients[2000];

int main(int argc, char** argv) {

    int MAXUSER = 2000;
    int MAXEVENTS = 10;
    int INS = 4;
    int QUEUESIZE = 100;

    if (argc != 2) {
        fprintf(stderr, "Usage : %s port.\n", argv[0]);
    }

    int sockfdListen;
    int port = atoi(port);
    if ((sockfdListen = socket_create(port)) < 0) {
        perror("socket_create");
        exit(1);
    } 

    DBG(GREEN"Server starts.\n"NONE);

    struct task_queue* taskQueue = (struct task_queue*)malloc(sizeof(struct task_queue));
    task_queue_init(taskQueue, QUEUESIZE);
    
    DBG(GREEN"task queue starts.\n"NONE);

    pthread_t* tid = (pthread_t*)calloc(INS, sizeof(pthread_t));
    
    for (int i = 0; i < INS; ++i) {
        pthread_create(tid + i, NULL, thread_work, (void*)taskQueue);
    }

    DBG(GREEN"thread starts.\n"NONE);

    int epollfd;
    if ((epollfd = epoll_create(1)) < 0) {
        perror("epoll_create");
        exit(1);
    }

    struct epoll_event ev, events[MAXEVENTS];
    ev.data.fd = sockfdListen;
    ev.events = EPOLLIN;
    
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfdListen, &ev)) {
        perror("epoll_ctl");
        exit(1);
    }
    
    DBG(GREEN"epoll listen starts.\n"NONE);

    for (;;) {
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (nfds <= 0) {
            perror("epoll_wait");
            exit(1);
        }
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            if (fd == sockfdListen && (events[i].events & EPOLLIN)) {
                int sockfd;
                if ((sockfd = accept(sockfdListen, NULL, NULL)) < 0) {
                    perror("accept");
                    exit(1);
                }
                clients[sockfd] = sockfd;
                ev.data.fd = sockfd;
                ev.events = EPOLLIN;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
                    perror("epoll_ctl");
                    exit(1);
                }
            } else {
                if (events[i].events & EPOLLIN) {
                    task_queue_push(taskQueue, &clients[fd]);
                }
            }
        }
    }
}