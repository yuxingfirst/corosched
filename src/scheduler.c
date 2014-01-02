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
    assert(coro == nil);

    coro->ready = READY;

}

scheduler* sched_init() 
{
    scheduler* sched = malloc(sizeof(scheduler));
    if(sched == nil) {
        return nil; 
    }
    main_coro = 
}

void add_coro(scheduler* sched, coroutine* coro) 
{
    TAILQ_INSERT_TAIL(&sched->wait_sched_queue, coro, ws_tqe);  
}
