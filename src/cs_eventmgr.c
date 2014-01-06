/*
 * coro_sched - A mini coroutine schedule framework
 * Copyright (C) 2014 xiongj(shervalx@gmail.com).
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

#include "cs_eventmgr.h"
#include "cs_coroutine.h"

rstatus eventmgr_init(eventmanager* eventmgr, int nevents) 
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
    eventmgr->events = cs_clloc(sizeof(struct epoll_event), eventmgr->nevent);
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
    return M_OK;
}

void register_event(eventmanager *eventmgr, event *ev)
{
    ASSERT(eventmgr);
    ASSERT(ev);
    ASSERT(ev->sockfd > 0);
    ASSERT(ev->coro); 

    struct epoll_event epevent;
    uint32_t events = 0;
    if(ev->mask & ReadMask) {
	events |= EPOLLIN;
    } 
    if(ev->mask & WriteMask) {
	events |= EPOLLOUT; 
    } 
    events |= EPOLLET;
    
    evevent.events = events;
    epevent.data.ptr = ev;

    epoll_ctl(eventmgr->epfd, EPOLL_CTL_MOD, ev->sockfd, &epevent); 

    
}






