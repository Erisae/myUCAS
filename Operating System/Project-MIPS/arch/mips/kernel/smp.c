#include "irq.h"

#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "lock.h"
#include "smp.h"
#include "test.h"
#define MBX0_OFFSET 0
#define MBX1_OFFSET 8
#define MBX2_OFFSET 16
#define MBX3_OFFSET 24
uint64_t mailbox_base = 0xffffffffbfe11120;

static void init_pcb_core1()
{
}

void smp_bootstrap(void)
{
    /*初始化pcb*/
    init_pcb(2);
    /*open interupt*/
    set_cp0_status((uint64_t)0x10008001 | (1 << 12)); /*可以响应时间中断*/
    while (1)
    {
        ;
    }
}

void multicore_start(void)
{
    while (1)
        ;
}

void ipi_write64(unsigned long arg1, void *arg2)
{
}

/*
 * Setup the PC, SP, and GP of a secondary processor and start it runing!
 */
void loongson3_boot_secondary()
{
    *(uint64_t *)(mailbox_base + MBX1_OFFSET) = (uint64_t)0xffffffffa0dc0000; //SP
    *(uint64_t *)(mailbox_base + MBX2_OFFSET) = (uint64_t)0xffffffffa0dc0000; //GP
    *(uint64_t *)(mailbox_base + MBX3_OFFSET) = (uint64_t)0;
    *(uint64_t *)(mailbox_base + MBX0_OFFSET) = (uint64_t)&smp_bootstrap;
}