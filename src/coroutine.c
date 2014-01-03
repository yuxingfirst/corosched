#include "coroutine.h"
#include "s"

static coroid_t coroidgen = 0;

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
    coro->cid = coroidgen++;
    void* stk = (void*)(coro + 1);   
    coro_create(&coro->ctx, fn, arg, stk, stack);
	coro->status = FREE;
    return coro;
}




