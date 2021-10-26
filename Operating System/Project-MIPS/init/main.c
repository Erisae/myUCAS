/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *         The kernel's entry, where most of the initialization work is done.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#include "fs.h"
#include "irq.h"
#include "test.h"
#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "syscall.h"
#include "smp.h"
#include "mm.h"
#include "mac.h"
#include "lock.h"
#include "mailbox.h"
#include "time.h"

int current_pid;
uint64_t exp_in = 0xffffffff80000180;
uint64_t tlb_in = 0xffffffff80000000;
extern uint64_t first_entry;
extern PTE_t ptable[128];
extern shm_t shared_mem[MAX_SHARED_NUM];

queue_t ready_queue[CORE_NUM];
queue_t block_queue[CORE_NUM];
queue_t exit_queue[CORE_NUM];
queue_t wait_recv_queue[CORE_NUM];
//desc_t * send_desc = (desc_t *)SEND_ADDR;
//desc_t * recv_desc = (desc_t *)RECV_ADDR;
desc_t send_desc[PNUM];
desc_t recv_desc[PNUM];
uint32_t recv_buffer[PNUM * PSIZE];
uint32_t buffer[PSIZE] = {0xffffffff, 0x5500ffff, 0xf77db57d, 0x00450008, 0x0000d400, 0x11ff0040, 0xa8c073d8, 0x00e00101, 0xe914fb00, 0x0004e914, 0x0000, 0x005e0001, 0x2300fb00, 0x84b7f28b, 0x00450008, 0x0000d400, 0x11ff0040, 0xa8c073d8, 0x00e00101, 0xe914fb00, 0x0801e914, 0x0000};

uint64_t kernel_stack[NUM_KERNEL_STACK];
int kernel_stack_count;
uint64_t user_stack[NUM_KERNEL_STACK];
int user_stack_count;

#define TASK_INIT (00)
#define TLBL 2
#define TLBS 3

void init_stack()
{
    int i;
    uint64_t kernel_top = 0xffffffffa0f00000;
    uint64_t user_top   = 0x7ffffff0;
    user_stack_count = 0;
    kernel_stack_count = 0;
    for (i = 0; i < NUM_KERNEL_STACK; i++)
    {
        kernel_stack[i] = kernel_top;
        kernel_top -= STACK_SIZE;
        kernel_stack_count++;
        user_stack[i] = user_top;
        user_top -= STACK_SIZE;
        user_stack_count++;
    }
}

static void init_memory()
{
    //init_page_table();
    //init_TLB();
    bzero(&ptable,sizeof(ptable));
    bzero(&shared_mem,sizeof(shared_mem));
}
void init_pcb(int core) //id status 入口地址
{
    /*初始化错油status*/
    if (core == 1)
    {
        int i = 0;
        while (i <= NUM_MAX_TASK + 1)
            pcb[i++].task_status = -1;
    }
    /*初始化queue*/
    queue_init(&ready_queue[core - 1]);
    queue_init(&block_queue[core - 1]);
    queue_init(&exit_queue[core - 1]);
    initial_cp0_status = (uint64_t)0x10008001 | (1 << 12);//status改变
    /*当前进程就是pcb[0]或者pcb[1]*/
    pcb[core - 1].pid = core - 1;
    pcb[core - 1].core =core - 1;
    pcb[core - 1].task_num = 0;
    current_running[core - 1] = &pcb[core - 1];
    current_running[core - 1]->task_status = TASK_RUNNING;
    current_running[core - 1]->kernel_state = TRUE;
    current_running[core - 1]->kernel_context.regs[29] = kernel_stack[core -1];
    /*test_shell是pcb[2],pid=2;test_multicore是pcb[3],pid=3*/
    int j = current_pid;
    pcb[j].pid = current_pid++;
    /*task_status*/
    pcb[j].task_status = TASK_READY;
    /*task_type*/
    pcb[j].task_type = USER_PROCESS;
    /*kernel_context初始化ra和sp*/
    pcb[j].kernel_context.regs[31] = (uint64_t)&first_entry; //首地址写入ra
    pcb[j].kernel_context.regs[29] = kernel_stack[j];
    /*kernel_context的cp0*/
    pcb[j].kernel_context.epc = (core == 1) ? (uint64_t)&test_shell : (uint64_t)&multicore_start;
    /*kernel的栈*/
    pcb[j].kernel_stack_top = kernel_stack[j];
    /*user_context初始化ra和sp*/
    pcb[j].user_context.regs[31] = (uint64_t)&first_entry; //首地址写入ra
    pcb[j].user_context.regs[29] = user_stack[j];
    /*user_context的cp0*/
    pcb[j].user_context.epc = (core == 1) ? (uint64_t)&test_shell : (uint64_t)&multicore_start;
    pcb[j].user_context.status = initial_cp0_status + 2; /*01+2*/
    /*user的栈*/
    pcb[j].user_stack_top = user_stack[j];
    /*用户*/
    pcb[j].kernel_state = FALSE;
    /*加入队列*/
    queue_push(&ready_queue[core - 1], &pcb[j]);
    pcb[j].in_queue = &ready_queue[core - 1];
    /*初始化锁*/
    pcb[j].lock_num = 0;
    /*初始化被我阻塞的*/
    queue_init(&pcb[j].blocked_by_me);
    /**/
    pcb[j].core = core-1;
    /*task_num*/
    pcb[j].task_num = 0;
}

static void init_exception_handler()
{
}

static void init_exception()
{
    /*例外处理代码拷贝到例外处理入?*/
    memcpy((void *)exp_in, (void *)exception_handler_entry, exception_handler_end - exception_handler_begin);
    memcpy((void *)tlb_in,(void *)TLBexception_handler_entry,TLBexception_handler_end - TLBexception_handler_begin);

    /*初始化例外向量表*/
    exception_handler[INT] = (uint64_t)handle_int;
    exception_handler[SYS] = (uint64_t)handle_syscall;
    exception_handler[TLBL] = (uint64_t)handle_tlb;
    exception_handler[TLBS] = (uint64_t)handle_tlb;
    int i;
    for (i = 0; i < 32; i++)
    {
        if (i != INT && i != SYS && i!=TLBL && i!=TLBS)
        {
            exception_handler[i] = (uint64_t)handle_other;
        }
    }

    /* set COUNT & set COMPARE */
    set_cp0_compare(TIMER_INTERVAL);
    set_cp0_count(0);
}

static void init_syscall(void)
{
    /*把syscall函数地址放到syscall数组里面*/
    syscall[SYSCALL_SLEEP] = (uint64_t(*)()) & do_sleep;
    syscall[SYSCALL_WRITE] = (uint64_t(*)()) & screen_write;
    syscall[SYSCALL_CURSOR] = (uint64_t(*)()) & screen_move_cursor;
    syscall[SYSCALL_REFLUSH] = (uint64_t(*)()) & screen_reflush;
    syscall[SYSCALL_BINSEM_GET] = (uint64_t(*)()) & binsem_get;
    syscall[SYSCALL_BINSEM_OP] = (uint64_t(*)()) & binsem_op;
    syscall[SYSCALL_PS] = (uint64_t(*)()) & do_process_show;
    syscall[SYSCALL_SPAWN] = (uint64_t(*)()) & do_spawn;
    syscall[SYSCALL_KILL] = (uint64_t(*)()) & do_kill;
    syscall[SYSCALL_SCREEN_CLEAR] = (uint64_t(*)()) & screen_clear;
    syscall[SYSCALL_SCREEN_SCROLL] = (uint64_t(*)()) & screen_scroll;
    syscall[SYSCALL_READ_SHELL_BUFF] = (uint64_t(*)()) & read_shell_buff;
    syscall[SYSCALL_MUTEX_LOCK_INIT] = (uint64_t(*)()) & do_mutex_lock_init;
    syscall[SYSCALL_MUTEX_LOCK_RELEASE] = (uint64_t(*)()) & do_mutex_lock_release;
    syscall[SYSCALL_MUTEX_LOCK_ACQUIRE] = (uint64_t(*)()) & do_mutex_lock_acquire;
    syscall[SYSCALL_EXIT] = (uint64_t(*)()) & do_exit;
    syscall[SYSCALL_WAITPID] = (uint64_t(*)()) & do_waitpid;
    syscall[SYSCALL_CONDITION_INIT] = (uint64_t(*)()) & do_condition_init;
    syscall[SYSCALL_CONDITION_WAIT] = (uint64_t(*)()) & do_condition_wait;
    syscall[SYSCALL_CONDITION_SIGNAL] = (uint64_t(*)()) & do_condition_signal;
    syscall[SYSCALL_CONDITION_BROADCAST] = (uint64_t(*)()) & do_condition_broadcast;
    syscall[SYSCALL_BARRIER_INIT] = (uint64_t(*)()) & do_barrier_init;
    syscall[SYSCALL_BARRIER_WAIT] = (uint64_t(*)()) & do_barrier_wait;
    syscall[SYSCALL_GETPID] = (uint64_t(*)()) & do_getpid;
    syscall[SYSCALL_SHELL_BUFF_EMPTY] = (uint64_t(*)()) & check_shell_buff;
    syscall[SYSCALL_MBOX_OPEN] = (uint64_t(*)()) & do_mbox_open;
    syscall[SYSCALL_MBOX_SEND] = (uint64_t(*)()) & do_mbox_send;
    syscall[SYSCALL_MBOX_RECV] = (uint64_t(*)()) & do_mbox_recv;
    syscall[SYSCALL_MBOX_CLOSE] = (uint64_t(*)()) & do_mbox_close;
    syscall[SYSCALL_CHANGE_CORE] = (uint64_t(*)()) & do_change_core;
    syscall[SYSCALL_GET_TIMER] = (uint64_t(*)()) & get_timer;
    syscall[SYSCALL_NET_RECV] = (uint64_t(*)()) & do_net_recv;
    syscall[SYSCALL_NET_SEND] = (uint64_t(*)()) & do_net_send;
    syscall[SYSCALL_FS_INIT] = (uint64_t(*)()) & do_mkfs;
    syscall[SYSCALL_FS_PRINT] = (uint64_t(*)()) & do_statfs;
    syscall[SYSCALL_FS_MKDIR] = (uint64_t(*)()) & mkdir;
    syscall[SYSCALL_FS_LS] = (uint64_t(*)()) & do_ls;
    syscall[SYSCALL_FS_CD] = (uint64_t(*)()) & do_cd;
    syscall[SYSCALL_FS_RMDIR] = (uint64_t(*)()) & rmdir;
    syscall[SYSCALL_FS_TOUCH] = (uint64_t(*)()) & do_touch;
    syscall[SYSCALL_FS_CAT] = (uint64_t(*)()) & do_cat;
    syscall[SYSCALL_FS_OPEN] = (uint64_t(*)()) & open;
    syscall[SYSCALL_FS_READ] = (uint64_t(*)()) & read;
    syscall[SYSCALL_FS_WRITE] = (uint64_t(*)()) & write;
    syscall[SYSCALL_FS_CLOSE] = (uint64_t(*)()) & close;
    syscall[SYSCALL_LN] = (uint64_t(*)()) & do_ln;
    syscall[SYSCALL_LNS] = (uint64_t(*)()) & do_ln_s;
    syscall[SYSCALL_FS_LL] = (uint64_t(*)()) & do_ll;
    syscall[SYSCALL_FS_SEARCH] = (uint64_t(*)()) & fsearch;
    syscall[SYSCALL_SHMGET] = (uint64_t(*)()) & do_shmget;
    syscall[SYSCALL_SHMAT] = (uint64_t(*)()) & do_shmat;
    syscall[SYSCALL_SHMDT] = (uint64_t(*)()) & do_shmdt;
    syscall[SYSCALL_SPAWN2] = (uint64_t(*)()) & do_spawn2;
}

static void init_lock(void)
{
    lock[7].key = 1345;
    queue_init(&lock[7].blocked_queue);
    lock[7].status = UNLOCKED;
}

/* [0] The beginning of everything >_< */
void __attribute__((section(".entry_function"))) _start(void)
{
    //asm_start();
    /* init stack space */
    init_stack();
    //printk("> [INIT] Stack heap initialization succeeded.\n");

    /* init Process Control Block */
    current_pid = 2;
    init_pcb(1);
    //printk("> [INIT] PCB initialization succeeded.\n");

    /* init interrupt */
    init_exception();
    //printk("> [INIT] Interrupt processing initialization succeeded.\n");

    init_memory();
    //printk("> [INIT] Virtual memory initialization succeeded.\n");
    // init system call table (0_0)
    /* init system call table */

    init_syscall();
    //printk("> [INIT] System call initialized successfully.\n");

    /* init screen */
    init_screen();
    printk("> [INIT] SCREEN initialization succeeded.\n");

    init_lock();
    //printk("> [INIT] LOCK initialization succeeded.\n");

    do_init_mac();
    //mbox_init();

    /* init filesystem */
    //read_super_block();
    /* wake up core1*/
    //loongson3_boot_secondary();
    int flag;
    //flag = do_mkfs();
    init_fds();
    set_cp0_status(initial_cp0_status); /*可以响应时间中断*/
    while (1)
    {
        //do_scheduler();
    };
}