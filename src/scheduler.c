#include "scheduler.h"
#include "coroutine.h"

rstatus coro_swapn(scheduler *sched, void (*fn)(void *arg), void *arg, size_t stacksize)
{
    assert(sched == nil);

    coroutine *coro = coro_alloc(fn, arg, stacksize);
    if(coro == nil) {
        return ERR;
    }
	coro_register(sched, coro);
    coro_ready(sched, coro);
    return OK;
}

void coro_ready(scheduler* sched, coroutine* coro)
{
    assert(coro->status == FREE || coro->status == RUN);
    coro->ready = READY;
	insert_tail(&sched->wait_sched_queue, coro);
}


void coro_yield(scheduler* sched)
{
	coro_ready(sched, sched->current_coro);
	coro_switch(sched->current_coro, sched->sched_coro);
}

void coro_exit(scheduler* sched) 
{
	sched->current_coro->status = EXIT;
	coro_switch(sched->current_coro, sched->sched_coro);
}

static void coro_register(scheduler* sched, coroutine* coro)
{
	if(sched->nallcoroutines % COROUTINE_SIZE == 0) {	//need expand allcoroutines
		allcoroutines = realloc(sched->allcoroutines, (sched->nallcoroutines + COROUTINE_SIZE) * sizeof(sched->allcoroutines));
		if(allcoroutines == nil) {
			fprintf(2, "%s\n", "coro_register realloc fail");
			abort();
		}
	}
	coro->alltaskslot = nallcoroutines;
	allcoroutines[sched->nallcoroutines++] = coro;
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
		c->status = RUN;
		sched->current_coro = c;
		coro_switch(sched_coro, sched->current_coro);
		assert(c == sched->current_coro);
		sched->current_coro = nil;
		if(c->status == EXIT) {	//need free
			i = c->alltaskslot;
			allcoroutines[i] = allcoroutines[--sched->nallcoroutines];
			allcoroutines[i]->alltaskslot = i;
			coro_dealloc(c);
		}
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
	sched_coro = coro_create(&sched_proc, sched, DEFAULT_STACK_SIZE);
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

