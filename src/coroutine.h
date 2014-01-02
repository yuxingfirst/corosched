#ifndef _COROUTINE_H_
#define _COROUTINE_H_ 1

#include "common.h"

typedef struct coroutine
{
	TAILQ_ENTRY(coroutine)     ws_tqe;    /* link in sheduler wait_shed_queue */
	coroid_t        cid;
	coro_context    ctx;
	uchar           *stk;
    size_t          stksize;
	int	            alltaskslot;
	int        	    exiting:1;
	int             ready:1;
	void            (*startfn)(void*);
    void            *startarg;
};

void coro_switch(coroutine *from, coroutine *to);
coroutine* coro_create(void (*fn)(void*), void *arg, size_t stack);

#endif	//_COROUTINE_H_
