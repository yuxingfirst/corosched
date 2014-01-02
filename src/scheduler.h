#ifndef _SHEDULER_H_
#define _SHEDULER_H_ 1

#include "common.h"

/*
 * the sheduler manager all the active coroutines
*/

enum 
{
    DEFAULT_STACK_SIZE = 8192
};

typedef struct scheduler 
{
    coroutine* allcoroutines; 
    long nallcoroutines;
	
	uint stop;
	
    coroutine* main_coro; 
    coroutine* running_coro; 

    coro_tqh wait_sched_queue;
};

coroutine* get_coro(scheduler* sched, coroid_t pid);
void add_coro(scheduler* sched, coroutine* coro);

void coro_yield(scheduler* sched, coroutine* coro);
void coro_ready(scheduler* sched, coroutine* coro)
void coro_exit(coroutine* coro);

rstatus coro_create(scheduler *shed, void (*fn)(void *arg), void *arg, size_t stacksize);

scheduler* sched_init();
int sched_run(scheduler* shed);
void sched_stop(scheduler* sched);

#endif 

