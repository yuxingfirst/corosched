#ifndef _COROUTINE_H_
#define _COROUTINE_H_ 1

#include "common.h"

static coro_id_t coroidgen;

typedef struct coroutine
{
	TAILQ_ENTRY(coroutine)     ws_tqe;           /* link in sheduler wait_shed_queue */
	
	coro_id_t cid;
	coro_context ctx;
	unsigned char        *stk;
    unsigned int        stksize;
	int	alltaskslot;
	int        	exiting;
	int        ready;
	void        (*startfn)(void*);
    void        *startarg;
};

void coro_switch(coroutine *from, coroutine *to);
int	 coro_create(void (*f)(void *arg), void *arg, unsigned int stacksize);
void coro_ready(coroutine *coro);

static coroutine* coro_alloc(void (*fn)(void*), void *arg, unsigned int stack);

#endif	//_COROUTINE_H_