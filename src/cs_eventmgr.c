/*
 * coro_sched - A mini coroutine schedule framework
 * Copyright (C) 2014 xiongj(yuxingfirst@gmail.com).
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

/*
 * the eventmanager implement a event driver lib based on epoll,
 * so we should build the coro_sched since linux 2.6+.
*/

#include "cs_eventmgr.h"
#include "cs_coroutine.h"
#include "cs_scheduler.h"

 struct eventmanager *g_eventmgr;

rstatus_t eventmgr_init(eventmanager* eventmgr, int nevents) 
{
    eventmgr = cs_alloc(sizeof(eventmanager));
    if(eventmgr == nil) {
        return M_ERR;
    }
    eventmgr->epfd = epoll_create(1024);	/*1024 will be ignored*/
    if(eventmgr->epfd == -1) {
        cs_free(eventmgr);
        return M_ERR;
    }
    eventmgr->nevent = nevents;
    eventmgr->events = cs_calloc(eventmgr->nevent, sizeof(*eventmgr->events));
    if(eventmgr->events == nil) {
        cs_free(eventmgr);
        return M_ERR;
    }	
    eventmgr->evmgr_coro = coro_alloc(&event_loop, eventmgr, DEFAULT_STACK_SIZE);
    if(eventmgr->evmgr_coro == nil) {
        cs_free(eventmgr);
        cs_free(eventmgr->events);
        return M_ERR; 
    }
    eventmgr->stop = 0;
    return M_OK;
}

void register_event(eventmanager *eventmgr, event *ev)
{
    ASSERT(eventmgr);
    ASSERT(ev);
    ASSERT(ev->sockfd > 0);
    ASSERT(ev->coro); 

    uint32_t events = 0;
    if(ev->mask & ReadMask) {
        events |= EPOLLIN;
    } 
    if(ev->mask & WriteMask) {
        events |= EPOLLOUT; 
    } 
    events |= EPOLLET;
    ev->events = events;

    struct epoll_event epevent;
    epevent.events = events;
    epevent.data.ptr = ev;

    epoll_ctl(eventmgr->epfd, EPOLL_CTL_ADD, ev->sockfd, &epevent); 

    coro_yield(ev->coro); 
}

void remove_event(eventmanager *eventmgr, event *ev)
{
    struct epoll_event ignored;
    epoll_ctl(eventmgr->epfd, EPOLL_CTL_DEL, ev->sockfd, &ignored);    
}

static void event_callback(struct event *ev, int events) 
{
    if(events & ReadMask) {
        ev->events |= ReadMask; 
    }
    if(events & WriteMask) {
        ev->events |= WriteMask; 
    }
    coro_ready(ev->coro);
    sched_run_once();
}

static void event_loop(void *arg) 
{
    eventmanager *eventmgr = (eventmanager*)arg;
    ASSERT(eventmgr);
    int nsd;
    while(!eventmgr->stop) {
        if(sched_has_task()) {
            nsd = epoll_wait(eventmgr->epfd, eventmgr->events, eventmgr->nevent, 0);
            if(nsd < 0) {
                log_error("epoll_wait occur error, errno:%d", errno); 
                break;
            }
            int i; 
            for(i=0;i<nsd;i++) {
                struct epoll_event *epev = &eventmgr->events[i];      
                event_callback(epev->data.ptr, epev->events);
            } 

            sched_run_once();
        }   
        nsd = epoll_wait(eventmgr->epfd, eventmgr->events, eventmgr->nevent, eventmgr->max_timeout);
        if(nsd < 0) {
            log_error("epoll_wait occur error, errno:%d", errno); 
            break;
        }
    }
    log_warn("event loop exit.");
}


