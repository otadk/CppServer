#ifndef _HEAD_H
#define _HEAD_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "color.h"
#include "common.h"
#include "thread_pool.h"

// gcc xxx.c -D _D to start Debug mode
#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#endif