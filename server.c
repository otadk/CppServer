#include "server.h"

int main() {

    int port = 7788; // 服务器端口

    // 创建线程池
    TaskQueue* taskQueue = (TaskQueue*)malloc(sizeof(TaskQueue));
    taskQueueInit(taskQueue);
    pthread_t* tids = (pthread_t*)calloc(4, sizeof(pthread_t));
    for (int i = 0; i < 4; ++i) {
        pthread_create(tids + i, NULL, serverRun, (void*)taskQueue);
    }

    // 创建套接字
    int sockfdListen = socketCreate(port);

    // 创建反应堆
    int epollfd = epoll_create(1);

    // 把套接字放入反应堆
    epollAddFd(sockfdListen, epollfd);

    // 把终端输入放入反应堆
    epollAddFd(0, epollfd);

    // 创建反应堆存储区域
    struct epoll_event events[10];

    // 创建用户哈希表
    HashMapNode** hashMap = newHashMap();

    // 创建用户名哈希集合
    HashSetNode** hashSet = newHashSet();

    for (int isStop = 0; isStop == 0;) {

        DBG(BLUE"<Loop> "GREEN"A loop starts.\n"NONE);

        // 反应堆阻塞，等待输入
        int nfds = epoll_wait(epollfd, events, 10, -1);

        for (int i = 0; i < nfds; ++i) {
            if (events[i].events & EPOLLIN) {
                int fd = events[i].data.fd;
                if (fd == sockfdListen) {
                    // 新建连接
                    epollAddFd(accept(sockfdListen, NULL, NULL), epollfd);

                    DBG(BLUE"<Accept>"GREEN"accept a msg\n"NONE);
                } else if (fd == 0) {
                    // 终端系统输入
                    char terminalIn[512];
                    read(fd, terminalIn, sizeof(terminalIn));
                    DBG(BLUE"<System>"GREEN" Command : %s"NONE, terminalIn);
                    if (terminalIn[0] == 'q' && terminalIn[1] == '\n') {
                        // 输入q表示结束程序
                        isStop = 1;
                        break;
                    }
                    // 这里可以拓展终端控制命令
                } else {
                    // 连接用户输入
                    Msg msgBuffer;
                    int recvSize = recv(fd, (void*)&msgBuffer, sizeof(Msg), 0);
                    if (recvSize <= 0) {
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                    }
                    Msg* msg = (Msg*)malloc(sizeof(Msg));
                    strcpy(msg->msg, msgBuffer.msg);
                    msg->type = msgBuffer.type;
                    // 线程池处理消息
                    Task* task = (Task*)malloc(sizeof(Task));
                    task->epollfd = epollfd;
                    task->fd = fd;
                    task->msg = msg;
                    task->hashMap = hashMap;
                    task->hashSet = hashSet;
                    taskQueuePush(taskQueue, task);
                }
            }
        }
    }
    return 0;
}

// 创建监听套接字
int socketCreate(int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0) {
        return -1;
    }
    if (listen(sockfd, 8) < 0) {
        return -1;
    }
    return sockfd;
}

// 创建哈希表
HashMapNode** newHashMap() {
    return (HashMapNode**)calloc(HASHSIZE, sizeof(HashMapNode*));
}

// 哈希表的哈希函数
int hashMapHash(int fd) {
    return fd % HASHSIZE;
}

// 向哈希表添加元素
void hashMapAdd(HashMapNode** hashMap, int fd, char* name) {
    int index = hashMapHash(fd);
    HashMapNode* addNode = (HashMapNode*)malloc(sizeof(HashMapNode));
    addNode->fd = fd;
    addNode->name = name;
    addNode->next = NULL;
    HashMapNode* node = hashMap[index];
    if (node == NULL) {
        hashMap[index] = addNode;
    } else {
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = addNode;
    }
}

// 向哈希表删除元素
void hashMapRemove(struct hashMapNode** hashMap, int fd) {
    int index = hashMapHash(fd);
    HashMapNode* node = hashMap[index];
    HashMapNode* lastNode = NULL;
    while (node != NULL) {
        if (node->fd == fd) {
            if (lastNode == NULL) {
                hashMap[index] = NULL;
            } else {
                lastNode->next = node->next;
            }
            free(node);
            return ;
        }
        lastNode = node;
        node = node->next;
    }    
}

// 获取哈希表元素
char* hashMapGet(struct hashMapNode** hashMap, int fd) {
    int index = hashMapHash(fd);
    HashMapNode* node = hashMap[index];
    while (node != NULL) {
        if (node->fd == fd) {
            return node->name;
        }
        node = node->next;
    }
    return NULL;
}

// 获取哈希表元素数量
int hashMapSize(struct hashMapNode** hashMap) {
    int size = 0;
    for (int i = 0; i < HASHSIZE; ++i) {
        HashMapNode* node = hashMap[i];
        while (node != NULL) {
            size += 1;
            node = node->next;
        }
    }
    return size;
}

// 获取哈希表下标集合
int* hashMapKeySet(struct hashMapNode** hashMap) {
    int size = hashMapSize(hashMap);
    int* keySet = (int*)calloc(size, sizeof(int));
    int index = 0;
    for (int i = 0; i < HASHSIZE; ++i) {
        HashMapNode* node = hashMap[i];
        while (node != NULL) {
            keySet[index] = node->fd;
            index += 1;
            node = node->next;
        }
    }
    return keySet;
}

// 创建哈希集合
HashSetNode** newHashSet() {
    return (HashSetNode**)calloc(HASHSIZE, sizeof(HashSetNode*));
}

// 哈希集合的哈希函数
int hashSetHash(char* name) {
    int index = 0;
    for (int i = 0; i < strlen(name); ++i) {
        index += name[index];
    }
    return index % HASHSIZE;
}

// 向哈希集合添加元素
void hashSetAdd(HashSetNode** hashSet, char* name) {
    int index = hashSetHash(name);
    HashSetNode* addNode = (HashSetNode*)malloc(sizeof(HashSetNode));
    addNode->name = name;
    addNode->next = NULL;
    HashSetNode* node = hashSet[index];
    if (node == NULL) {
        hashSet[index] = addNode;
    } else {
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = addNode;
    }
}

// 向哈希结合移除元素
void hashSetRemove(HashSetNode** hashSet, char* name) {
    int index = hashSetHash(name);
    HashSetNode* node = hashSet[index];
    HashSetNode* lastNode = NULL;
    while (node != NULL) {
        if (strcmp(node->name, name) == 0) {
            if (lastNode == NULL) {
                hashSet[index] = NULL;
            } else {
                lastNode->next = node->next;
            }
            free(node);
            return ;
        }
        lastNode = node;
        node = node->next;
    }
}

// 判断哈希集合中是否存在某元素
int hashSetContains(HashSetNode** hashSet, char* name) {
    int index = hashSetHash(name);
    HashSetNode* node = hashSet[index];
    while (node != NULL) {
        if (strcmp(node->name, name) == 0) {
            return 1;
        }
        node = node->next;
    }
    return 0;
}

// 线程池初始化
void  taskQueueInit(TaskQueue* taskQueue) {
    taskQueue->size  = TASKQUEUESIZE;
    taskQueue->total = taskQueue->head = taskQueue->tail = 0;
    taskQueue->data  = (void**)calloc(TASKQUEUESIZE, sizeof(void*));
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
}

// 向线程池添加任务
void taskQueuePush(TaskQueue* taskQueue, void* data) {
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

// 线程池推出任务，若没有任务会阻塞
// 注意这里用while而不是if是因为醒来之后有可能被别的线程抢了导致total还是0
void* taskQueuePop(TaskQueue* taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
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

// 线程执行函数
void* serverRun(void* taskData) {
    pthread_detach(pthread_self());
    TaskQueue* taskQueue = (TaskQueue*)taskData;
    for (;;) {
        Task* task = (Task*)taskQueuePop(taskQueue);
        DBG(BLUE"<Loop>"GREEN"A thread starts.\n"NONE);
        Msg* msg = task->msg;
        Msg feedBack;
        if (msg->type == 0) {
            // 登陆
            if (hashSetContains(task->hashSet, msg->msg) == 0) {
                DBG(BLUE"<Login>"GREEN"Login success!\n"NONE);
                feedBack.type = 3; //成功
                char* name = (char*)calloc(20, sizeof(char));
                strcpy(name, msg->msg);
                hashMapAdd(task->hashMap, task->fd, name);
                hashSetAdd(task->hashSet, name);
                strcpy(feedBack.msg, "Login success!");
                send(task->fd, (void*)&feedBack, sizeof(feedBack), 0);

            } else {
                DBG(BLUE"<Login>"GREEN"Login fail!\n"NONE);
                feedBack.type = 4; // 失败
                strcpy(feedBack.msg, "Login fail!");
                send(task->fd, (void*)&feedBack, sizeof(feedBack), 0);

            }
        } else if (msg->type == 1) {
            // 登出
            feedBack.type = 3;
            strcpy(feedBack.msg, "Login out success!");
            send(task->fd, (void*)&feedBack, sizeof(feedBack), 0);

            hashMapRemove(task->hashMap, task->fd);
            hashSetRemove(task->hashSet, msg->msg);
            epoll_ctl(task->epollfd, EPOLL_CTL_DEL, task->fd, NULL);
            DBG(BLUE"<Logout>"GREEN"Login out success! %s\n"NONE, msg->msg);
            
        } else if (msg->type == 2) {
            // 发送消息
            DBG(BLUE"<Send>"GREEN"Send success!\n"NONE);
            feedBack.type = 3;
            strcpy(feedBack.msg, "Send success!");
            send(task->fd, (void*)&feedBack, sizeof(feedBack), 0);

            int* fdSet = hashMapKeySet(task->hashMap);
            int size = hashMapSize(task->hashMap);
            feedBack.type = 5; // 消息
            
            char* name = hashMapGet(task->hashMap, task->fd);
            char msgBuffer[500];
            strcpy(msgBuffer, msg->msg);
            sprintf(feedBack.msg, "%s::%s", name, msgBuffer);
            for (int i = 0; i < size; ++i) {
                if (fdSet[i] == task->fd) {
                    continue;
                }
                send(fdSet[i], (void*)&feedBack, sizeof(feedBack), 0);
            }
        }
        free(msg);
        free(task);
    }
}

// 向一个反应堆加入一个文件描述符
void epollAddFd(int fd, int epollfd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}