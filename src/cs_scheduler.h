/*
 * coro_sched - A mini coroutine schedule framework
 * Copyright (C) 2014 yuxingfirst@gmail.com.
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

#ifndef _SHEDULER_H_
#define _SHEDULER_H_ 1

#include "cs_common.h"
#include "cs_coroutine.h"

/*
 * the sheduler manager all the active coroutines
*/

typedef struct scheduler 
{
    coro_context main_coro;
    coroutine **allcoroutines; 
    int nallcoroutines;
    int stop;
    coroutine *sched_coro; 
    coroutine *current_coro; 
    coro_tqh wait_sched_queue;
    bool is_parallel_sched;
}scheduler;

void sched_register_coro(coroutine* coro);

void yield_and_scheduler();
coroutine* get_coro(coroid_t pid);
void sched_stop(scheduler *sched);
bool sched_has_task();
static void sched_run(void *arg);
static void master_start(void *arg);

void parallel_start(void *arg);
void parallel_sched_run(void *arg);

typedef struct salfschedulebackadapter
{
    coroutine *scbd_coro; 
   	int readfd;
   	int writefd; 
}salfschedulebackadapter;

static void scheduleback_run(void *arg);

rstatus_t env_init();
rstatus_t env_run();
void env_stop();

static void insert_tail(coro_tqh *queue, coroutine* coro);
static void insert_head(coro_tqh *queue, coroutine* coro);
static bool empty(coro_tqh *queue);
static coroutine* pop(coro_tqh *queue);

#endif 

