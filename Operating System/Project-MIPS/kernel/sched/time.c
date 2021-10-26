#include "time.h"
#include "sched.h"

uint64_t time_elapsed = 0;

static int MHZ = 300;

uint64_t get_ticks()
{
    return time_elapsed;
}

uint64_t get_timer_kernel()
{
    int core;
    return time_elapsed / (150000);
}

uint64_t get_timer()
{
    int core;
    core=get_cp0_prid();
    current_running[core]->user_context.regs[2] = time_elapsed / (150000);
    return time_elapsed / (150000);
}

void latency(uint64_t time)
{
    uint64_t begin_time = get_timer();
    
    while (get_timer() - begin_time < time)
    {
    };
    return;
}