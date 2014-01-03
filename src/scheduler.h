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
	
	int stop;
	
    coroutine main_coro; 
    coroutine* current_coro; 

    coro_tqh wait_sched_queue;
};

/*
   *  Give up the cpu temporarily and wait reschedule later
   */
void coro_yield(scheduler* sched);

void coro_ready(scheduler* sched, coroutine* coro)

/*
  *  Current coro exit from the scheduler and main coro take charge of execute
  */
void coro_exit(scheduler* sched);

/*
  * Create a coroutine and add to schedule queue
  */
rstatus coro_create(scheduler *shed, void (*fn)(void *arg), void *arg, size_t stacksize);

scheduler* sched_init();
int sched_run(scheduler* shed);
void sched_stop(scheduler* sched);
void sched_func(void *arg);

coroutine* get_coro(scheduler* sched, coroid_t pid);

void insert_tail(coro_tqh *queue, coroutine* coro);
void insert_head(coro_tqh *queue, coroutine* coro);
int  empty(coro_tqh *queue);


#endif 

