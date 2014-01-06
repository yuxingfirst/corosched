/*
 * coro_sched - A mini coroutine schedule framework
 * Copyright (C) 2014 yuxingfirst@gmail.com.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

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


