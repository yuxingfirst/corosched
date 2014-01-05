#include "cs_coroutine.h"


static coroid_t coroidgen = 0;

void coro_switch(coroutine *from, coroutine *to) {
    ASSERT(from != nil); 
    ASSERT(to != nil);
    coro_transfer(&from->ctx, &to->ctx);
}

coroutine* coro_alloc(void (*fn)(void*), void *arg, size_t stack) 
{
    coroutine *co = nil;
    co = cs_alloc(sizeof(coroutine) + stack);
    if(co == nil) {
        log_error("%s", "coro_alloc fail");
        return nil;
    } 
    memset(co, 0, sizeof(coroutine));
    co->cid = coroidgen++;
    void* stk = (void*)(co + 1);   
    coro_create(&co->ctx, fn, arg, stk, stack);
	co->status = M_FREE;
    co->sched = nil;
    co->parallel_id = M_INVALID_PARALLEL_ID;
    return co;
}

void coro_dealloc(coroutine *c) 
{
    cs_free(c); 
}


