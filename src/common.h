#ifndef _COMMON_H_
#define _COMMON_H_ 1

#include "queue.h"
#include "coro.h"
#include "log.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <sys/stat.h>

#define uint unsigned int 
#define uchar unsigned char

#define nil ((void*)0)
#define OK 0
#define ERR -1

#define cstatus int
#define FREE 1
#define READY 2
#define RUN 3
#define EXIT 4

#define COROUTINE_SIZE 64
#define SCHED_CORO_ID 0

#define coroid_t int
#define rstatus int

typedef struct coro_tqh coro_tqh;
TAILQ_HEAD(coro_tqh, coroutine);

#endif  //_COMMON_H_
