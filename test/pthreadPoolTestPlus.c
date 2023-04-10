#include "../common/head.h"
void* pthreadPoolTestPlusRun(void *threadData);
void* pthreadPoolTestPlusRun(void *threadData) {
    pthread_detach(pthread_self());
    struct task_queue* taskQueue = (struct task_queue*)threadData;
    for (;;) {
        char* buffer = (char*)task_queue_pop(taskQueue);
        printf("%s\n", buffer);
    }
}


// 创建线程池
struct task_queue* createTaskQueue() {
    struct task_queue* taskQueue = (struct task_queue*)malloc(sizeof(struct task_queue));
    task_queue_init(taskQueue, 10);
    pthread_t* tids = (pthread_t*)calloc(4, sizeof(pthread_t));
    for (int i = 0; i < 4; ++i) {
        pthread_create(tids + i, NULL, pthreadPoolTestPlusRun, (void*)taskQueue);
    }
}

int main() {
    
    FILE *fp;

    struct task_queue* taskQueue = createTaskQueue();

    for (int i = 0; i < 1e2; ++i) {

        if ((fp = fopen("FopenFile.txt", "r")) == NULL) {
            perror("fopen");
            exit(1);
        }

        char buffer[1024];
        fread(buffer, 1024, 1024, fp);
        task_queue_push(taskQueue, buffer);
        
        fclose(fp);
    }

    sleep(1);

    return 0;
}