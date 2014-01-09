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

struct scheduler 
{
    coro_context main_coro;
    int stop;
    coroutine *sched_coro; 
    coroutine *current_coro; 
    coro_tqh wait_sched_queue;
    bool is_parallel_sched;
}scheduler;

void sched_register_coro(coroutine* coro);
coroutine* sched_get_coro(coroid_t pid);
void sched_run_once();
bool sched_has_task();
static void sched_run(void *arg);

void* parallel_main(void *arg);
static void parallel_run(void *arg);

struct salfschedulebackadapters
{
    coroutine *scbd_coro; 
   	int readfd;
   	int writefd; 
}salfschedulebackadapter;

static void scheduleback_run(void *arg);

rstatus_t env_init();
rstatus_t env_run();
void env_stop();

 void insert_tail(coro_tqh *queue, coroutine* coro);
 void insert_head(coro_tqh *queue, coroutine* coro);
 bool empty(coro_tqh *queue);
 coroutine* pop(coro_tqh *queue);

#endif 

