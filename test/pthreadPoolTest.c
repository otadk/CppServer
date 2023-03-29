#include "../common/head.h"

int main() {

    int INS = 3;
    int MAX = 100;
    
    FILE *fp;
    pthread_t tid[INS];
    struct task_queue *taskQueue = (struct task_queue *)malloc(sizeof(struct task_queue));
    task_queue_init(taskQueue, MAX);
    
    for (int i = 0; i < INS; ++i) {
        pthread_create(&tid[i], NULL, thread_run, (void*)taskQueue);
    }

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