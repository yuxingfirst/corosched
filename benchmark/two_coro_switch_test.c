#include "cs_scheduler.h"
#include "cs_common.h"

int counter = 0;

void func(void *arg) {
   while(1) {
        if(++counter > 1000) {
            env_stop(); 
        } 
        log(LOG_INFO, "counter:%d", counter);
        coro_yield(g_mastersched->current_coro); 
   } 
}

int main(void) 
{
    if( log_init(LOG_INFO, "sched_switch_t.log") < 0 ) {
        printf("init log error.\n");
        return 0; 
    }
    if( env_init() != 0) {
        return 0;
    }

    coro_spawn(g_mastersched, &func, NULL, 8192);  
    coro_spawn(g_mastersched, &func, NULL, 8192);  

    log_warn("Init scheduler success, start...");
    int rs = env_run();
    log_warn("Scheduler stop, status:%d", rs);
    return 0;
}

//compile: gcc two_coro_switch_test.c -I../include/ -L../lib/ -l corosched_s -DCORO_ASM -l pthread -o twocoroswitchtest

