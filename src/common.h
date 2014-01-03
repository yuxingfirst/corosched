#ifndef _COMMON_H_
#define _COMMON_H_ 1

#include "queue.h"
#include "coro.h"

#include "string.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define uint unsigned int 
#define uchar unsigned char

#define nil NULL
#define OK 0
#define ERR -1

#define cstatus int
#define FREE 1
#define READY 2
#define RUN 3
#define EXIT 4

#define COROUTINE_SIZE 1024

#define coroid_t long
#define rstatus int

typedef struct coroutine coroutine;
typedef struct coro_tqh coro_tqh;

TAILQ_HEAD(coro_tqh, coroutine);

#endif  //_COMMON_H_
