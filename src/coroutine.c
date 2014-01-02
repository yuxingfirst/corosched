#include "coroutine.h"
#include "s"

static coroid_t coroidgen = 1;

void coro_switch(coroutine *from, coroutine *to) {
    assert(from != nil); 
    assert(to != nil);
    coro_transfer(&from->ctx, &to->ctx);
}

coroutine* coro_create(void (*fn)(void*), void *arg, size_t stack) 
{
    coroutine *coro = nil;
    coro = malloc(sizeof(coroutine) + stack);
    if(coro == nil) {
        fprintf(2, "%s\n", "coro_alloc fail.");
        return nil;
    } 
    memset(coro, 0, sizeof(coroutine));
    coro->cid = ++coroidgen;
    coro->stk = (uchar*)(coro + 1);   
    coro->stksize = stack;
    coro->startfn = fn; 
    coro->startarg = arg;
    coro->ready = UNREADY;
    coro_create(&coro->ctx, coro->startfn, coro->startarg, coro->stk, coro->stksize);
    return coro;
}




