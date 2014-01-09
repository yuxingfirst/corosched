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
 #include "cs_scheduler.h"

static coroid_t coroidgen = M_SCHED_CORO_ID + 1;

rstatus_t coro_spawn(scheduler *sched, void (*fn)(void *arg), void *arg, size_t stacksize)
{
    coroutine *coro = coro_alloc(fn, arg, stacksize);
    if(coro == nil) {
        return M_ERR;
    }
    coro->status = M_FREE;
    coro->sched = sched;
    sched_register_coro(sched, coro);
    coro_ready(coro);
    return M_OK;
}

void coro_ready(coroutine* coro)
{
    ASSERT(coro->status == M_FREE || coro->status == M_RUN);
    coro->status = M_READY;
    insert_tail(&coro->sched->wait_sched_queue, coro);
}

void coro_ready_immediatly(coroutine* coro) 
{
    ASSERT(coro->status == M_FREE || coro->status == M_RUN);
    coro->status = M_READY;
    insert_head(&coro->sched->wait_sched_queue, coro);
}

void coro_exit(coroutine *co) 
{
    ASSERT(co);
    ASSERT(co == co->sched->current_coro);
    co->status = M_EXIT;
    coro_switch(co, co->sched->sched_coro);
}

void coro_yield(coroutine *co)
{
    ASSERT(co);
    ASSERT(co == co->sched->current_coro);
    coro_ready(co);
    coro_switch(co->sched->current_coro, co->sched->sched_coro);
}

rstatus coro_switch_to_master(coroutine *c)
{
    ASSERT(c != nil);
    while(true) {
        c->sched = g_mastersched;
        ssize n = send(g_schedulerbackadapter->writefd, (const void*)c, sizeof(struct coroutine*), MSG_DONTWAIT);
        if(n == -1 && errno == EINTR) {
            continue;
        }        
        if(n != sizeof(struct coroutine)) {
            c->sched = g_parallelsched;
            return M_ERR;
        }
        return M_OK;
    }
}

rstatus coro_switch_to_parallel(coroutine *co)
{
    ASSERT(co);
    if(!co->need_parallel) {
        return M_ERR;
    }   
    coro_ready_immediatly(co);
    coro_yield(co);
    return M_OK;
}

rstatus coro_sent_parallel(coroutine *c)
{
    ASSERT(c);
    ASSERT(c->need_parallel);
    if(pthread_mutex_trylock(&g_mutex) == 0) {
        c->sched = g_parallelsched;
        insert_head(&c->sched->wait_sched_queue, c);
        pthread_mutex_unlock(&g_mutex); 
        return M_OK;
    }
    c->sched = g_mastersched;
    return M_ERR; 
}

bool coro_runin_parallel(coroutine *c)
{
    ASSERT(c);
    return c->sched->is_parallel_sched;
}

void coro_switch(coroutine *from, coroutine *to) {
    ASSERT(from); 
    ASSERT(to);
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
    co->need_parallel = false;
    return co;
}

void coro_dealloc(coroutine *c) 
{
    cs_free(c); 
}