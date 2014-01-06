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
    int                         parallel_id;
};

/*
  * coroutine switch
 */
void coro_switch(coroutine *from, coroutine *to);

/*
  * alloc coroutine
  *
  * Return nil if occur error
 */
coroutine* coro_alloc(void (*fn)(void*), void *arg, size_t stack);

void coro_dealloc(coroutine *c);

#endif	//_COROUTINE_H_
