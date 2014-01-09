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

#ifndef _COROUTINE_H_
#define _COROUTINE_H_ 1

#include "cs_common.h"

struct coroutine 
{
	TAILQ_ENTRY(coroutine)     	ws_tqe;    /* link in sheduler wait_shed_queue */
	coroid_t        			cid;
	coro_context    			ctx;
	int	            			alltaskslot;
	cstatus_t 					status;	
    scheduler                   *sched;   //my scheduler
    bool              need_parallel;
	struct rbnode	node;
};

rstatus_t coro_spawn(scheduler *sched, void (*fn)(void *arg), void *arg, size_t stacksize);
rstatus coro_switch_to_master(coroutine *c);
rstatus coro_switch_to_parallel(coroutine *c);
rstatus coro_sent_parallel(coroutine *c);

void coro_ready(coroutine* coro);
void coro_ready_immediatly(coroutine* coro);
void coro_exit();
void coro_yield(coroutine *co);

bool coro_runin_parallel(coroutine *c);

coroutine* coro_alloc(void (*fn)(void*), void *arg, size_t stack);
void coro_switch(coroutine *from, coroutine *to);
void coro_dealloc(coroutine *c);

#endif	//_COROUTINE_H_
