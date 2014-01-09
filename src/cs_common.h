/*
 * coro_sched - A mini coroutine schedule framework
 * Copyright (C) 2014 xiongj(yuxingfirst@gmail.com).
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

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

typedef long msg_seqid_t;

typedef int coroid_t;
typedef int rstatus_t;
typedef int cstatus_t;

typedef struct scheduler scheduler;
typedef struct coroutine coroutine;
typedef struct eventmanager eventmanager;
typedef struct salfschedulebackadapter salfschedulebackadapter;

typedef struct coro_tqh coro_tqh;
TAILQ_HEAD(coro_tqh, coroutine);

extern struct scheduler *g_mastersched;
extern struct scheduler *g_parallelsched;
extern struct eventmanager *g_eventmgr;
extern struct salfschedulebackadapter *g_schedulebackadapter;
extern pthread_mutex_t g_mutex;

enum 
{
    DEFAULT_STACK_SIZE = 8192
};

enum
{
    ReadMask = 0x1,
    WriteMask = 0x2
};

#endif  //_COMMON_H_

