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

__thread struct scheduler *g_mastersched;
int n_max_procs = -1;

rstatus_t coro_spawn(void (*fn)(void *arg), void *arg, size_t stacksize)
{
    coroutine *coro = coro_alloc(fn, arg, stacksize);
    if(coro == nil) {
        return M_ERR;
    }
    coro->sched = g_mastersched;
	coro_register(coro);
    coro_ready(coro);
    return M_OK;
}

void coro_ready(coroutine* coro)
{
    ASSERT(coro->status == M_FREE || coro->status == M_RUN);
    coro->status = M_READY;
	insert_tail(&coro->sched->wait_sched_queue, coro);
}

void coro_yield()
{
	coro_ready(g_mastersched, g_mastersched->current_coro);
	coro_switch(g_mastersched->current_coro, g_mastersched->sched_coro);
}

void coro_exit() 
{
	g_mastersched->current_coro->status = M_EXIT;
	coro_switch(g_mastersched->current_coro, g_mastersched->sched_coro);
}

static void coro_register(coroutine* coro)
{
	if(g_mastersched->nallcoroutines % COROUTINE_SIZE == 0) {	//need expand allcoroutines
		g_mastersched->allcoroutines = realloc(g_mastersched->allcoroutines, (g_mastersched->nallcoroutines + COROUTINE_SIZE) * sizeof(g_mastersched->allcoroutines));
        ASSERT(g_mastersched->allcoroutines != nil);
	}
	coro->alltaskslot = g_mastersched->nallcoroutines;
	g_mastersched->allcoroutines[g_mastersched->nallcoroutines++] = coro;
}

static void sched_run(void *arg) 
{
	int i = -1;
	while(!g_mastersched->stop) {
		coroutine *c = pop(&g_mastersched->wait_sched_queue);
		if(c == nil) {
			break;
		}
        if(c->parallel_id != M_INVALID_PARALLEL_ID) {   //need switch to parallel sched
            //todo..
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

rstatus_t switchto_parallel_sched(coroutine *c)
{
    if(c->parallel_id == M_INVALID_PARALLEL_ID) {
        c->parallel_id = get_curr_parallelid(); 
    } 
    if(!valid_parallelid(c->parallel_id)) {
        return M_EINVALID_PARALLELID; 
    }
    scheduler *parallel_sched = g_parallel_scheds + c->parallel_id;
    ASSERT(parallel_sched);
    c->sched = parallel_sched;
    insert_tail_salf(&parallel_sched->wait_sched_queue, c);
    return M_OK;
}

rstatus_t switchto_master_sched(coroutine *c)
{
    //todo..
    return M_OK;
}

void* parallel_main(void *arg)
{
    scheduler *psched = (scheduler*)arg;
    ASSERT(psched != nil);
    coro_transfer(&psched->main_coro, &psched->sched_coro->ctx);
    return NULL;
}

void parallel_sched_run(void *arg)
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
rstatus_t env_init() 
{
    n_max_procs = cs_get_nprocs();
    if(n_max_procs <= 0) {
        return M_ERR; 
    }
    g_parallel_scheds = cs_alloc(n_max_procs * sizeof(scheduler)); 

    g_mastersched = cs_alloc(sizeof(scheduler)); 
    if(g_mastersched == nil) {
        log_error("init scheduler malloc fail.");
        return nil; 
    }
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

static int  empty(coro_tqh *queue) 
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

