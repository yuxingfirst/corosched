#include "cs_scheduler.h"
#include "cs_log.h"

scheduler *g_sched;

int counter = 0;

void func(void *arg) {
   while(1) {
        if(++counter > 15000000) {
            sched_stop(g_sched); 
        } 
        coro_yield(g_sched); 
   } 
}

int main(void) 
{
    if( log_init(LOG_INFO, "sched_switch_t.log") < 0 ) {
        printf("init log error.\n");
        return 0; 
    }
    g_sched = sched_init(); 
    if(g_sched == NULL) {
        printf("init sched error.\n");
        return 0; 
    }

    coro_spawn(g_sched, &func, NULL, 8192);  
    coro_spawn(g_sched, &func, NULL, 8192);  

    log_warn("Init scheduler success, start...");
    int rs = sched_run(g_sched);
    log_warn("Scheduler stop, status:%d", rs);
    return 0;
}
