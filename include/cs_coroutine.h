#ifndef _COROUTINE_H_
#define _COROUTINE_H_ 1

#include "cs_common.h"

typedef struct coroutine coroutine;
struct coroutine 
{
	TAILQ_ENTRY(coroutine)     	ws_tqe;    /* link in sheduler wait_shed_queue */
	coroid_t        			cid;
	coro_context    			ctx;
	int	            			alltaskslot;
	cstatus_t 					status;	
};

/*
  * coroutine switch
 */
void coro_switch(coroutine *from, coroutine *to);

/*
  * alloc coroutine
  *
  * Return nil if occur error
 */
coroutine* coro_alloc(void (*fn)(void*), void *arg, size_t stack);

void coro_dealloc(coroutine *c);

#endif	//_COROUTINE_H_