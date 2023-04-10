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

void* thread_task(void* threadData) {
    pthread_detach(pthread_self());
    struct task_queue* taskQueue = (struct task_queue*) threadData;
    while (1) {
        void* data = task_queue_pop(taskQueue);
        printf("%s", (char*)data);
    }
    free(taskQueue);
}
