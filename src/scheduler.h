#ifndef _SHEDULER_H_
#define _SHEDULER_H_ 1

#include "common.h"

/*
 * the sheduler manager all the active coroutines
*/

typedef struct coroutine coroutine;

TAILQ_HEAD(coro_tqh, coroutine);
typedef struct coro_tqh coro_tqh;

typedef struct scheduler 
{
    coroutine* allcoroutines; 
    long nallcoroutines;
	
	unsigned int stop;
	
    coroutine* main_coro; 
    coroutine* running_coro; 

    coro_tqh wait_sched_queue;
};

coroutine* get_coroutine(scheduler* sched, coro_id_t pid);

void yield(scheduler* sched, coroutine* coro);

scheduler* sched_init();
int sched_run(scheduler* shed);
void sched_stop(scheduler* sched);

#endif 

