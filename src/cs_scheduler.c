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

#include "cs_scheduler.h"

struct scheduler *g_mastersched;
struct scheduler *g_parallelsched;
struct salfschedulerbackadapter *g_schedulebackadapter;

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void sched_register_coro(coroutine* coro)
{
     if(g_mastersched->nallcoroutines % COROUTINE_SIZE == 0) {        //need expand allcoroutines
                g_mastersched->allcoroutines = realloc(g_mastersched->allcoroutines, (g_mastersched->nallcoroutines + COROUTINE_SIZE) * sizeof(g_mastersched->allcoroutines));
        ASSERT(g_mastersched->allcoroutines != nil);
        }
        coro->alltaskslot = g_mastersched->nallcoroutines;
        g_mastersched->allcoroutines[g_mastersched->nallcoroutines++] = coro;
}

coroutine* sched_get_coro(coroid_t pid)
{
    return nil; 
}

void sched_run_once()
{
	while(!g_mastersched->stop) {
		if(!sched_has_task()) {
			break;
		}		
		coroutine *c = pop(&g_mastersched->wait_sched_queue);
		if(c == nil) {
			break;
		}
		if(c->status == M_EXIT) {
			continue;
		}	

		coro_ready(g_mastersched->current_coro);

		if(c->parallel_id != M_INVALIED_PARALLEL_ID) {
			coro_switch_to_parallel(c);
			continue;
		}
		coroutine *my = g_mastersched->current_coro;
		g_mastersched->current_coro = c;
		coro_switch(my, g_mastersched->current_coro);		
		break;
	}	
}

bool sched_has_task() {
    return !empty(&g_mastersched->wait_sched_queue); 
}

static void sched_run(void *arg) 
{
	int i = -1;
	while(!g_mastersched->stop) {
		coroutine *c = pop(&g_mastersched->wait_sched_queue);
		if(c == nil) {
			break;
		}
        if(c->parallel_id != M_INVALID_PARALLEL_ID) {   
		    if(coro_sent_parallel(c) != M_ERR) {
                coro_ready_immediatly(c);   
            }
            continue; 
        }
		c->status = M_RUN;
		g_mastersched->current_coro = c;
		coro_switch(g_mastersched->sched_coro, g_mastersched->current_coro);
		ASSERT(c == g_mastersched->current_coro);
		g_mastersched->current_coro = nil;
		if(c->status == M_EXIT) {	//need free
			i = c->alltaskslot;
			g_mastersched->allcoroutines[i] = g_mastersched->allcoroutines[--g_mastersched->nallcoroutines];
			g_mastersched->allcoroutines[i]->alltaskslot = i;
			coro_dealloc(c);
		}
	}
    log_warn("no tasks, exit scheduler");
	coro_transfer(&g_mastersched->sched_coro->ctx, &g_mastersched->main_coro);
}

void* parallel_main(void *arg)
{
    scheduler *psched = (scheduler*)arg;
    ASSERT(psched != nil);
    coro_transfer(&psched->main_coro, &psched->sched_coro->ctx);
    return NULL;
}

static void parallel_run(void *arg)
{
    scheduler *psched = (scheduler*)arg;
    while(!psched->stop) {
        if(empty_salf(&psched->wait_sched_queue)) {
            usleep(1000 * 500); 
            continue;
        } 
        coroutine *co = pop_salf(&psched->wait_sched_queue);
        ASSERT(co != nil);
        if(co->parallel_id != psched->parallelnum) {
            log_error("not match parallelid, coro->id:%d, coro->parallel_id:%d, sched->parallelnum:%d", co->cid, co->parallel_id, psched->parallelnum); 
            continue;
        }
        coro_switch(psched->sched_coro, co);
        rstatus rs = switchto_master_sched(co);
        if(rs != M_OK) {
            log_error("switch back to master sched fail, coro->id:%d, sched->parallelnum:%d", co->cid, psched->parallelnum); 
        }
    }
    coro_transfer(&psched->main_coro, &psched->sched_coro->ctx);
}

static void scheduleback_run(void *arg)
{

}

rstatus_t env_init() 
{
    g_mastersched = cs_alloc(sizeof(scheduler)); 
    g_parallel_sched = cs_alloc(sizeof(scheduler)); 
    g_schedulebackadapter = cs_alloc(sizeof(salfschedulebackadapter));

    if(g_mastersched == nil || g_parallel_sched == nil || g_schedulebackadapter= nil) {
        log_error("init scheduler malloc fail.");
        cs_free(g_mastersched);
        cs_free(g_parallel_sched);
        cs_free(g_schedulebackadapter);
        return nil; 
    }
    {
        g_mastersched->allcoroutines = nil;
        g_mastersched->nallcoroutines = 0;
        g_mastersched->stop = 0;
        g_mastersched->sched_coro = coro_alloc(&sched_proc, nil, DEFAULT_STACK_SIZE);
        if(g_mastersched->sched_coro == nil || g_mastersched->sched_coro->cid != SCHED_CORO_ID) {
            log_error("init scheduler alloc sched_coro fail");
            return nil;
        }
        g_mastersched->current_coro = nil;
        TAILQ_INIT(&g_mastersched->wait_sched_queue);
    }

    {
        g_parallel_sched->stop = 0;
        g_parallel_sched->sched_coro = coro_alloc(&parallel_sched_run, g_parallel_sched, DEFAULT_STACK_SIZE);
        g_parallel_sched->current_coro = nil;
        TAILQ_INIT(&g_parallel_sched->wait_sched_queue);
    }

    {
        int fd[2];
        socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
        g_schedulebackadapter->scbd_coro = coro_alloc(&scheduleback_run, nil, DEFAULT_STACK_SIZE);
        g_schedulebackadapter->readfd = fd[0];
        g_schedulebackadapter->writefd = fd[1];
    }

	return M_OK;
}

rstatus_t env_run() 
{
	ASSERT(g_mastersched != nil);
	coro_transfer(&g_mastersched->main_coro, &g_mastersched->sched_coro->ctx);
	return M_OK;
}

void env_stop() 
{
	g_mastersched->stop = 1;
}

static void insert_tail(coro_tqh *queue, coroutine* coro)
{
	TAILQ_INSERT_TAIL(queue, coro, ws_tqe);
}

static void insert_head(coro_tqh *queue, coroutine* coro)
{
	TAILQ_INSERT_HEAD(queue, coro, ws_tqe);
}

static bool empty(coro_tqh *queue) 
{
	return TAILQ_EMPTY(queue);
}

static coroutine* pop(coro_tqh *queue) 
{
	coroutine *head = TAILQ_FIRST(queue);
	if(head == nil) {
		return nil;
	}
	TAILQ_REMOVE(queue, head, ws_tqe);
	return head;
}

