#ifndef _SHEDULER_H_
#define _SHEDULER_H_ 1

#include "cs_common.h"
#include "cs_coroutine.h"

/*
 * the sheduler manager all the active coroutines
*/

enum 
{
    DEFAULT_STACK_SIZE = 8192
};

typedef struct scheduler scheduler;
struct scheduler 
{
    coro_context main_coro;
    coroutine **allcoroutines; 
    int nallcoroutines;
    int stop;
    coroutine *sched_coro; 
    coroutine *current_coro; 
    coro_tqh wait_sched_queue;
    int parallelnum;    //if master scheduler, it is -1.
};

rstatus_t coro_spawn(void (*fn)(void *arg), void *arg, size_t stacksize);
void coro_yield();
void coro_ready(coroutine* coro);
void coro_exit();

coroutine* get_coro(coroid_t pid);
void sched_stop(scheduler *sched);
static void coro_register(coroutine* coro);
static void sched_run(void *arg);

rstatus_t switchto_parallel_sched(coroutine *c);
rstatus_t switchto_master_sched(coroutine *c);
void parallel_start(void *arg);
void parallel_sched_run(void *arg);

rstatus_t env_init();
rstatus_t env_run();
void env_stop();

static void master_start(void *arg);

static void insert_tail(coro_tqh *queue, coroutine* coro);
static void insert_head(coro_tqh *queue, coroutine* coro);
static int  empty(coro_tqh *queue);
static coroutine* pop(coro_tqh *queue);

#endif 

