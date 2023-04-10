#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

// 颜色
#define NONE      "\e[0;m"
#define BLACK     "\e[0;30m"
#define L_BLACK   "\e[1;30m"
#define RED       "\e[0;31m"
#define L_RED     "\e[1;31m"
#define GREEN     "\e[0;32m"
#define L_GREEN   "\e[1;32m"
#define BROWN     "\e[0;33m"
#define YELLOW    "\e[1;33m"
#define BLUE      "\e[0;34m"
#define L_BLUE    "\e[1;34m"
#define PINK      "\e[0;35m"
#define L_PINK    "\e[1;35m"
#define CYAN      "\e[0;36m"
#define L_CYAN    "\e[1;36m"
#define GRAY      "\e[0;37m"
#define WHITE     "\e[1;37m"
#define BOLD      "\e[1m"
#define UNDERLINE "\e[4m"
#define BLINK     "\e[5m"
#define REVERSE   "\e[7m"
#define HIDE      "\e[8m"
#define CLEAR     "\e[2J"
#define CLRLINE   "\r\e[K"

// gcc xxx.c -D _D to start Debug mode
#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

int socketCreate(int port);

// 哈希
#define HASHSIZE 20

typedef struct hashMapNode {
    int fd;
    char* name;
    struct hashMapNode* next;
} HashMapNode;

HashMapNode** newHashMap();
int hashMapHash(int fd);
void hashMapAdd(HashMapNode** hashMap, int fd, char* name);
void hashMapRemove(HashMapNode** hashMap, int fd);
char* hashMapGet(HashMapNode** hashMap, int fd);
int hashMapSize(HashMapNode** hashMap);
int* hashMapKeySet(HashMapNode** hashMap);

typedef struct hashSetNode {
    char* name;
    struct hashSetNode* next;
} HashSetNode;

HashSetNode** newHashSet();
int hashSetHash(char* name);
void hashSetAdd(HashSetNode** hashSet, char* name);
void hashSetRemove(HashSetNode** hashSet, char* name);
int hashSetContains(HashSetNode** hashSet, char* name);

// 线程池
#define TASKQUEUESIZE 10

typedef struct taskQueue {
    int head, tail, size, total;
    void** data;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

void  taskQueueInit(TaskQueue* taskQueue);
void  taskQueuePush(TaskQueue* taskQueue, void* data);
void* taskQueuePop(TaskQueue* taskQueue);
void* serverRun(void* taskData);

typedef struct Message {
    int type;
    char msg[512];
} Msg;

typedef struct Task {
    int epollfd;
    int fd;
    Msg* msg;
    HashMapNode** hashMap;
    HashSetNode** hashSet;
} Task;

void epollAddFd(int fd, int epollfd);