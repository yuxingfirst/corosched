#include "cs_scheduler.h"

rstatus_t coro_spawn(scheduler *sched, void (*fn)(void *arg), void *arg, size_t stacksize)
{
    assert(sched != nil);
    coroutine *coro = coro_alloc(fn, arg, stacksize);
    if(coro == nil) {
        return M_ERR;
    }
	coro_register(sched, coro);
    coro_ready(sched, coro);
    return M_OK;
}

void coro_ready(scheduler* sched, coroutine* coro)
{
    assert(coro->status == M_FREE || coro->status == M_RUN);
    coro->status = M_READY;
	insert_tail(&sched->wait_sched_queue, coro);
}


void coro_yield(scheduler* sched)
{
	coro_ready(sched, sched->current_coro);
	coro_switch(sched->current_coro, sched->sched_coro);
}

void coro_exit(scheduler* sched) 
{
	sched->current_coro->status = M_EXIT;
	coro_switch(sched->current_coro, sched->sched_coro);
}

static void coro_register(scheduler* sched, coroutine* coro)
{
	if(sched->nallcoroutines % COROUTINE_SIZE == 0) {	//need expand allcoroutines
		sched->allcoroutines = realloc(sched->allcoroutines, (sched->nallcoroutines + COROUTINE_SIZE) * sizeof(sched->allcoroutines));
		if(sched->allcoroutines == nil) {
            log_error("register coroutine,realloc fail");
			abort();
		}
	}
	coro->alltaskslot = sched->nallcoroutines;
	sched->allcoroutines[sched->nallcoroutines++] = coro;
}

static void sched_proc(void *arg) 
{
	int i = -1;
	scheduler *sched = (scheduler*)arg;
	while(!sched->stop) {
		coroutine *c = pop(&sched->wait_sched_queue);
		if(c == nil) {
			break;
		}
		c->status = M_RUN;
		sched->current_coro = c;
		coro_switch(sched->sched_coro, sched->current_coro);
		assert(c == sched->current_coro);
		sched->current_coro = nil;
		if(c->status == M_EXIT) {	//need free
			i = c->alltaskslot;
			sched->allcoroutines[i] = sched->allcoroutines[--sched->nallcoroutines];
			sched->allcoroutines[i]->alltaskslot = i;
			coro_dealloc(c);
		}
	}
    log_warn("no tasks, exit scheduler");
	coro_transfer(&sched->sched_coro->ctx, &sched->main_coro);
}

scheduler* sched_init() 
{
    scheduler* sched = malloc(sizeof(scheduler));
    if(sched == nil) {
        log_error("init scheduler malloc fail.");
        return nil; 
    }
	sched->allcoroutines = nil;
    sched->nallcoroutines = 0;
	sched->stop = 0;
	sched->sched_coro = coro_alloc(&sched_proc, sched, DEFAULT_STACK_SIZE);
	if(sched->sched_coro == nil || sched->sched_coro->cid != SCHED_CORO_ID) {
        log_error("init scheduler alloc sched_coro fail");
		return nil;
	}
	sched->current_coro = nil;
    TAILQ_INIT(&sched->wait_sched_queue);
	return sched;
}

rstatus_t sched_run(scheduler* sched) 
{
	assert(sched != nil);
	coro_transfer(&sched->main_coro, &sched->sched_coro->ctx);
	return M_OK;
}

void sched_stop(scheduler* sched) 
{
	sched->stop = 1;
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

