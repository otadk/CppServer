#include "head.h"

extern char* data[2000];
extern int epollfd;
extern pthread_mutex_t mutex[2000];

void task_queue_init(struct task_queue* taskQueue, int size) {
    taskQueue->size  = size;
    taskQueue->total = taskQueue->head = taskQueue->tail = 0;
    taskQueue->data  = (void**)calloc(size, sizeof(void*));
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
}

void task_queue_push(struct task_queue* taskQueue, void *data) {
    pthread_mutex_lock(&taskQueue->mutex);
    if (taskQueue->total == taskQueue->size) {
        DBG(YELLOW"<push> taskQueue is full.\n"NONE);
        if (taskQueue->size > 1e5) {
            DBG(RED"<push> taskQueue is full.\n"NONE);
            return ;
        }
        taskQueue->size = taskQueue->size + taskQueue->size;
        void** data = taskQueue->data;
        taskQueue->data = (void**)calloc(taskQueue->size, sizeof(void*));
        for (int i = 0; i < taskQueue->size / 2; ++i) {
            taskQueue->data[i] = data[i];
        }
    }
    DBG(GREEN"<push> push task.\n"NONE);
    taskQueue->data[taskQueue->tail] = data;
    taskQueue->tail = (taskQueue->tail + 1) % taskQueue->size;
    taskQueue->total += 1;
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

void* task_queue_pop(struct task_queue* taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
    // 这里用while而不是if是因为醒来之后有可能被别的线程抢了导致total还是0
    while (taskQueue->total == 0) {
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }
    DBG(GREEN"<pop> pop task.\n"NONE);
    void* result = taskQueue->data[taskQueue->head];
    taskQueue->head = (taskQueue->head + 1) % taskQueue->size;
    taskQueue->total -= 1;
    pthread_mutex_unlock(&taskQueue->mutex);
    return result;
}

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
    if ((recvsize = recv(fd, buffer, sizeof(buffer), 0)) <= 0) {
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
