#include "barrier.h"
#include "sched.h"

void do_barrier_init(barrier_t *barrier, int goal)
{
    barrier->expect_num = goal;
    barrier->now_num = 0;
    queue_init(&barrier->block_queue);
}

void do_barrier_wait(barrier_t *barrier)
{
    int core;
    core = get_cp0_prid();
    barrier->now_num++;
    if (barrier->now_num < barrier->expect_num)
        {
            current_running[core]->in_queue = &barrier->block_queue;
            do_block(&barrier->block_queue);
        }
    else
    {
        do_unblock_all(&barrier->block_queue);
        barrier->now_num = 0;
        do_scheduler();
    }
}