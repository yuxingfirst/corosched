#ifndef _SHEDULER_H_
#define _SHEDULER_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

/*
 * the sheduler manager all the active coroutines
*/

#define COROUTINE_SIZE 1024
#define coro_id_t long

typedef struct Coroutine Coroutine;

//instead with list_head
typedef struct CoroList
{
    Coroutine* head;
    Coroutine* tail;
};

typedef struct Sheduler 
{
    Coroutine* allcoroutines; 
    long nallcoroutines;

    Coroutine* shed_coro; 
    Coroutine* running_coro; 

    CoroList wait_shed_queue;


};

Coroutine* get_coroutine(Sheduler* shed, coro_id_t pid);

void coro_list_push(CoroList* list, Coroutine* paddcoro);
Coroutine* coro_list_pop(CoroList* list);
void coro_list_del(CoroList* list, Coroutine* pdelcoro);

#ifdef __cplusplus
}
#endif
#endif 

