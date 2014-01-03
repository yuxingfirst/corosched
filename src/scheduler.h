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
    coro_context main_coro;
    coroutine* allcoroutines; 
    int nallcoroutines;
    int stop;
    coroutine* sched_coro; 
    coroutine* current_coro; 
    coro_tqh wait_sched_queue;
};

/*
   *  Give up the cpu temporarily and wait reschedule later
   */
void coro_yield(scheduler* sched);

void coro_ready(scheduler* sched, coroutine* coro);

/*
  *  Current coro exit from the scheduler and main coro take charge of execute
  */
void coro_exit(scheduler* sched);

/*
  * Create a coroutine and add to schedule queue
  */
rstatus coro_swapn(scheduler *shed, void (*fn)(void *arg), void *arg, size_t stacksize);

static void coro_register(scheduler* sched, coroutine* coro);

scheduler* sched_init();
rstatus sched_run(scheduler* sched);
void sched_stop(scheduler* sched);
static void sched_proc(void *arg);

coroutine* get_coro(scheduler* sched, coroid_t pid);

static void insert_tail(coro_tqh *queue, coroutine* coro);
static void insert_head(coro_tqh *queue, coroutine* coro);
static int  empty(coro_tqh *queue);
static coroutine* pop(coro_tqh *queue);

#endif 

