#ifndef _HEAD_H
#define _HEAD_H

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
#include <sys/select.h>
#include "color.h"
#include "common.h"
#include "thread_pool.h"
#include "hash.h"
#include "chat.h"
#include "hashMap.h"

// gcc xxx.c -D _D to start Debug mode
#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#endif
