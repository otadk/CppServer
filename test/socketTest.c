#include "../common/head.h"

int main(int argc, char** argv) {
    
    int INS = 5;
    int MAX = 100;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s port.\n", argv[0]);
        exit(1);
    }

    pthread_t tid[INS];

    struct task_queue* taskQueue = (struct task_queue*)malloc(sizeof(struct task_queue));
    task_queue_init(taskQueue, MAX);
    for (int i = 0; i < INS; ++i) {
        pthread_create(&tid[i], NULL, thread_run, (void*)taskQueue);
    }

    int port = atoi(argv[1]);
    int sockfdListen;

    if ((sockfdListen = socket_create(port)) < 0) {
        perror("socket_create");
        exit(1);
    }

    int bufferId = 0;
    char buffer[MAX][4096];
    fd_set rfds;
    struct hashNode** HashSet = newHashSet(MAX);

    DBG(GREEN"Localhost::%d Start listening\n"NONE, port);

    while (1) {

        FD_ZERO(&rfds);
        FD_SET(sockfdListen, &rfds);
        
        int* clients = keySet(HashSet, MAX);
        int nfds = sockfdListen + 1;
        for (int i = 0; i < getTotal(HashSet, MAX); ++i) {
            if (clients[i] >= nfds) {
                nfds = clients[i] + 1;
            }
            FD_SET(clients[i], &rfds);
        }
        
        int selectCount = select(nfds, &rfds, NULL, NULL, NULL);
        if (selectCount < 0) {
            perror("select");
            exit(1);
        }

        DBG(GREEN"Select!\n"NONE);

        if (FD_ISSET(sockfdListen, &rfds)) {

            DBG(GREEN"New connection!\n"NONE);

            int sockfd;
            if ((sockfd = accept(sockfdListen, NULL, NULL)) < 0) {
                perror("accept");
                exit(1);
            }
            add(HashSet, sockfd, MAX);
        }

        free(clients);
        clients = keySet(HashSet, MAX);
        for (int i = 0; i < getTotal(HashSet, MAX); ++i) {
            if (FD_ISSET(clients[i], &rfds)) {
                int recvSize = recv(clients[i], buffer[bufferId], 1024, 0);
                if (recvSize <= 0) {
                    close(clients[i]);
                    HashSetRemove(HashSet, clients[i], MAX);
                } else {
                    task_queue_push(taskQueue, buffer[bufferId]);
                    bufferId = (bufferId + 1) % MAX;
                }
            }
        }
        free(clients);
    }
}