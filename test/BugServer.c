#include "../common/head.h"

struct myChat_task {
    int epollfd;
    int fd;
    int type;
};


void* myChatRun(void* taskData) {
    pthread_detach(pthread_self());
    struct task_queue* taskQueue = (struct task_queue*)taskData;
    for (;;) {
        struct myChat_task* task = (struct myChat_task*)task_queue_pop(taskQueue);
        if (task->type == 1) {
            printf("accept\n");
            int sockfd = accept(task->fd, NULL, NULL);
            if (sockfd < 0) {
                perror("accept");
                exit(1);
            }
            struct epoll_event event;
            event.data.fd = sockfd;
            event.events = EPOLLIN;
            epoll_ctl(task->epollfd, EPOLL_CTL_ADD, sockfd, &event);
        } else if (task->type == 2) {
            struct chat_msg msg;
            recv(task->fd, (void*)&msg, sizeof(msg), 0);
            printf("%d\n", msg.type);
            if (msg.type == CHAT_SIGNOUT) {
                msg.type = CHAT_ACK;
                strcpy(msg.msg, "end!");
                send(task->fd, (void*)&msg, sizeof(msg), 0);
                epoll_ctl(task->epollfd, EPOLL_CTL_DEL, task->fd, NULL);
                close(task->fd);
            } else if (msg.type == CHAT_SIGNUP) {
                msg.type = CHAT_ACK;
                strcpy(msg.msg, "sign up!");
                send(task->fd, (void*)&msg, sizeof(msg), 0);
            } else if (msg.type == CHAT_SIGNIN) {
                msg.type = CHAT_ACK;
                strcpy(msg.msg, "sign in!");
                send(task->fd, (void*)&msg, sizeof(msg), 0);
            } else if (msg.type == CHAT_SENDMSG) {
                msg.type = CHAT_ACK;
                printf(BLUE"<MSG>%s"YELLOW"%s\n"NONE, msg.name, msg.msg);
                strcpy(msg.msg, "send msg!");
                send(task->fd, (void*)&msg, sizeof(msg), 0);
            }
        }

        // free(task);
    }
}



int main() {

    int port = 7788;

    struct task_queue* taskQueue = (struct task_queue*)malloc(sizeof(struct task_queue));
    task_queue_init(taskQueue, 10);
    
    pthread_t* tids = (pthread_t*)calloc(4, sizeof(pthread_t));
    
    for (int i = 0; i < 4; ++i) {
        pthread_create(tids + i, NULL, myChatRun, (void*)taskQueue);
    }

    int sockfdListen = socket_create(port);
    if (sockfdListen < 0) {
        perror("socket_create");
        exit(1);
    }

    int epollfd  = epoll_create(1);
    if (epollfd < 0) {
        perror("epoll_create");
        exit(1);
    }

    struct epoll_event event, events[100];
    event.data.fd = sockfdListen;
    event.events = EPOLLIN;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfdListen, &event)) {
        perror("epoll_ctl");
        exit(1);
    }

    event.data.fd = 0;
    event.events = EPOLLIN;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, 0, &event)) {
        perror("epoll_ctl");
        exit(1);
    }

    for (;;) {
        DBG(BLUE"one loop\n"NONE);
        int end = 0;
        int nfds = epoll_wait(epollfd, events, 100, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            exit(1);
        }
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            if (events[i].events & EPOLLIN) {
                if (fd == 0) {
                    // char terminalIn[512];
                    // read(fd, terminalIn, sizeof(terminalIn));
                    // if (terminalIn[0] == 'q' && terminalIn[1] == '\n') {
                    //     end = 1;
                    //     break;
                    // }
                    continue;
                }
                struct myChat_task* task = (struct myChat_task*)malloc(sizeof(struct myChat_task));
                task->fd = fd;
                task->epollfd = epollfd;
                if (fd == sockfdListen) {
                    // 新连接
                    task->type = 1;
                } else {
                    // 客户端输入
                    task->type = 2;
                }
                task_queue_push(taskQueue, task);
            }
        }
        if (end == 1) {
            break;
        }
    }

    free(tids);
    task_queue_clear(taskQueue);

    return 0;
}