#include "scheduler.h"
#include "coroutine.h"

rstatus coro_create(scheduler *sched, void (*fn)(void *arg), void *arg, size_t stacksize)
{
    assert(sched == nil);

    coroutine *coro = coro_create(fn, arg, stacksize);
    if(coro == nil) {
        return ERR;
    }
    coro_ready(sched, coro);
    return OK;
}

void coro_ready(scheduler* sched, coroutine* coro)
{
    assert(coro->status != READY);
    coro->ready = READY;
	add_coro(sched, coro);
}


void coro_yield(scheduler* sched)
{
	coro_ready(sched, sched->current_coro);
	coro_switch(sched->current_coro, sched->main_coro);
}


void coro_exit(scheduler* sched) 
{
	assert(sched->current_coro->exiting != 1);
	sched->current_coro->exiting = 1;
	coro_switch(sched->current_coro, sched->main_coro);
}

void sched_func(void *arg) 
{
	scheduler *sched = (scheduler*)arg;
	
	while(!sched->stop && !empty(&sched->wait_sched_queue)) {
		
	}
	
}

scheduler* sched_init() 
{
    scheduler* sched = malloc(sizeof(scheduler));
    if(sched == nil) {
        return NULL; 
    }
	sched->allcoroutines = nil;
    sched->nallcoroutines = 0;
	sched->stop = 0;
	sched_coro = coro_create(&sched_func, sched, DEFAULT_STACK_SIZE);
	if(sched_coro == nil || sched_coro->cid != SCHED_CORO_ID) {
		return NULL;
	}
	sched->current_coro = nil;
	TAILQ_HEAD_INITIALIZER(sched->wait_sched_queue);
	return sched;
}

rstatus sched_run(scheduler* sched) 
{
	assert(sched != nil);
	coro_transfer(&sched->main_coro, &sched->sched_coro->ctx);
	return OK;
}

void sched_stop(scheduler* sched) 
{
	sched->stop = 1;
}

void insert_tail(coro_tqh *queue, coroutine* coro)
{
	TAILQ_INSERT_TAIL(queue, coro, ws_tqe);
}

void insert_head(coro_tqh *queue, coroutine* coro)
{
	TAILQ_INSERT_HEAD(queue, coro, ws_tqe);
}

int  empty(coro_tqh *queue) 
{
	return TAILQ_EMPTY(queue);
}
