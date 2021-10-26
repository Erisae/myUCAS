#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"
#include "screen.h"
#include "mac.h"

/* exception handler */
uint64_t exception_handler[32];

/* used to init PCB */
uint64_t initial_cp0_status;

// extern void do_shell();

int *tlb_e = (int *)0x123456;

static void irq_timer()
{
    uint64_t ds_begin = 0, ds_end = 0, ds_cost = 0;
    screen_reflush();
    /* increase global time counter */
    time_elapsed += TIMER_INTERVAL;
    /* sched.c to do scheduler */
    //ds_begin = get_cp0_count();
    do_scheduler();
    //ds_cost -= get_cp0_count();
    //screen_cursor_x = 1;
    //screen_cursor_y = 9;
    // ds_end = get_cp0_count();
    // ds_cost = (ds_end > ds_begin) ? ds_end - ds_begin : 0;
    // printk("\n\n\n\n\nThe scheduler to thread[%d] time cost is %u cycles.", current_running->cursor_y - 4, ds_cost);
    //screen_cursor_x = current_running->cursor_x;
    //screen_cursor_y = current_running->cursor_y;
}
void other_exception_handler()
{
    int core;
    core = get_cp0_prid();
    uint64_t status, cause, badvaddr;
    status = get_cp0_status();
    cause = get_cp0_cause();
    badvaddr = get_cp0_badvaddr();
    printk("exception status  :%u\n", status);
    printk("exception cause   :%u\n", cause);
    printk("exception badvaddr:%u\n", badvaddr);
    current_running[core]->user_context.epc += 4;
    /*print STATUS*/
    /*print CASUE*/
    /*print EPC*/
    return;
}

void interrupt_helper(uint64_t status, uint64_t cause) /*不能直接用irq_timer*/
{
    if (((cause >> 8) & 0xff) == 0x80) /*时钟中断*/
        irq_timer();
    else if((cause >> 8) & 0x10 ) /*网卡中断，注意这时候可能int8也是1*/
        mac_irq_handle();
    else
        other_exception_handler();
}

void check_kernel()
{
    int core;
    core = get_cp0_prid();
    if (current_running[core]->kernel_state == FALSE)
        *tlb_e = 1;
    return;
}

void check_user()
{
    int core;
    core = get_cp0_prid();
    if (current_running[core]->kernel_state == TRUE)
        *tlb_e = 1;
    return;
}