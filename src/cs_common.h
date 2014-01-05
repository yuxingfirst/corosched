#ifndef _COMMON_H_
#define _COMMON_H_ 1

#include "cs_queue.h"
#include "cs_coro.h"
#include "cs_log.h"
#include "cs_util.h"

#include <stdint.h>
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
#include <pthread.h>

#define nil ((void*)0)

#define M_OK 0
#define M_ERR -1
#define M_ENOMEM -2
#define M_EINVALID_PARALLELID -3

#define M_FREE 1
#define M_READY 2
#define M_RUN 3
#define M_EXIT 4

#define M_COROUTINE_SIZE 64
#define M_SCHED_CORO_ID 0
#define M_INVALID_PARALLEL_ID -1 

typedef long msg_seqid_t;

typedef int coroid_t;
typedef int rstatus_t;
typedef int cstatus_t;

typedef struct scheduler scheduler;
typedef struct coroutine coroutine;

typedef struct coro_tqh coro_tqh;
TAILQ_HEAD(coro_tqh, coroutine);

extern __thread struct scheduler *g_mastersched;
extern __thread struct scheduler **g_parallel_scheds;

extern int current_parallelid;
extern int n_max_procs;

#endif  //_COMMON_H_
