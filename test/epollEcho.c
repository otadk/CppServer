#include "../common/head.h"

int clients[2000];
char* data[2000];
int epollfd;
pthread_mutex_t mutex[2000];

void* thread_run(void* threadData) {
    pthread_detach(pthread_self());
    struct task_queue* taskQueue = (struct task_queue*) threadData;
    while (1) {
        void* data = task_queue_pop(taskQueue);
        printf("%s", (char*)data);
    }
}

void do_work(int fd) {
    char buffer[4096] = {0};
    int recvsize = -1;
    int index = strlen(data[fd]);
    if ((recvsize = recv(fd, buffer, sizeof(buffer), 0)) <= 0 || (buffer[0] == 'q')) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        DBG(RED"<Close> : connection close\n"NONE);
        close(fd);
        return ;
    }
    pthread_mutex_lock(&mutex[fd]);
    DBG(GREEN"<Recv> recv data.\n"NONE);
    for (int i = 0; i < recvsize; ++i) {
        if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            data[fd][index] = buffer[i] - 'A' + 'a';
        } else if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            data[fd][index] = buffer[i] - 'a' + 'A';
        } else {
            data[fd][index] = buffer[i];
            if (buffer[i] == '\n') {
                DBG(GREEN"<Success> send data.\n"NONE);
                send(fd, data[fd], index, 0);
            }
        }
        index += 1;
    }
    pthread_mutex_unlock(&mutex[fd]);
}

void* thread_work(void* threadData) {
    pthread_detach(pthread_self());
    struct task_queue* taskQueue = (struct task_queue*) threadData;
    while (1) {
        int* fd = task_queue_pop(taskQueue);
        do_work(*fd);
    }
}

int main(int argc, char** argv) {

    int MAXUSER = 2000;
    int MAXEVENTS = 10;
    int INS = 4;
    int QUEUESIZE = 100;

    if (argc != 2) {
        fprintf(stderr, "Usage : %s port.\n", argv[0]);
    }

    int sockfdListen;
    int port = atoi(argv[1]);
    if ((sockfdListen = socket_create(port)) < 0) {
        perror("socket_create");
        exit(1);
    } 

    DBG(GREEN"<Init>Server starts.\n"NONE);

    struct task_queue* taskQueue = (struct task_queue*)malloc(sizeof(struct task_queue));
    task_queue_init(taskQueue, QUEUESIZE);
    
    DBG(GREEN"<Init>task queue starts.\n"NONE);

    pthread_t* tid = (pthread_t*)calloc(INS, sizeof(pthread_t));
    
    for (int i = 0; i < INS; ++i) {
        pthread_create(tid + i, NULL, thread_work, (void*)taskQueue);
    }

    DBG(GREEN"<Init>thread starts.\n"NONE);

    for (int i = 0; i < MAXUSER; ++i) {
        pthread_mutex_init(&mutex[i], NULL);
        data[i] = (char*)calloc(4096, sizeof(char));
    }

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
    
    DBG(GREEN"<Init>epoll listen starts.\n"NONE);

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
                ev.events = EPOLLIN | EPOLLET;
                make_nonblock(sockfd);
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