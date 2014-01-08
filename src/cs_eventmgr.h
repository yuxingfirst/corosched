/*
 * coro_sched - A mini coroutine schedule framework
 * Copyright (C) 2014 xiongj(shervalx@gmail.com).
 * 
 * the eventmanager implement a event driver lib based on epoll,
 * so we should build the coro_sched since linux 2.6+.
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

#ifndef _CS_EVENTMGR_H_
#define _CS_EVENTMGR_H_ 1

#include "cs_common.h"
#include <sys/epoll.h>

typedef struct event {
	uint32_t mask;
	int sockfd;
	int events;
	coroutine *coro;
}event;

struct eventmanager {
	int epfd;  /* epoll device */
	struct epoll_event *events;	/* epoll event */
	int nevent;	/* # epoll event */	
	int max_timeout; /* epoll wait max timeout in msec */
	coroutine *evmgr_coro; 
	int stop;
};

rstatus eventmgr_init(eventmanager *eventmgr);
void register_event(eventmanager *eventmgr, event *ev);
void remove_event(eventmanager *eventmgr, event *ev);

static void event_loop(void *arg);
static void event_callback(const struct *events, int nevent);

#endif	//_CS_EVENTMGR_H_
