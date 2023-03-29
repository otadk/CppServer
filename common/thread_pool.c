#include "head.h"

void task_queue_init(struct task_queue* taskQueue, int size) {
    taskQueue->size  = size;
    taskQueue->total = taskQueue->head = taskQueue->tail = 0;
    taskQueue->data  = (void **)calloc(size, sizeof(void *));
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
}

void task_queue_push(struct task_queue* taskQueue, void *data) {
    if (taskQueue->total == taskQueue->size) {
        DBG(YELLOW"<push> taskQueue is full.\n"NONE);
        return ;
    }
    pthread_mutex_lock(&taskQueue->mutex);
    DBG(GREEN"<push> push task.\n"NONE);
    taskQueue->data[taskQueue->tail] = data;
    taskQueue->tail = (taskQueue->tail + 1) % taskQueue->size;
    taskQueue->total += 1;
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

void *task_queue_pop(struct task_queue* taskQueue) {
    if (taskQueue->total == 0) {
        DBG(YELLOW"<pop> taskQueue is empty.\n"NONE);
        return ;
    }
    pthread_mutex_lock(&taskQueue->mutex);
    DBG(GREEN"<pop> pop task.\n"NONE);
    void *result = taskQueue->data[taskQueue->head];
    taskQueue->head = abs(taskQueue->head - 1);
    taskQueue->total -= 1;
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}