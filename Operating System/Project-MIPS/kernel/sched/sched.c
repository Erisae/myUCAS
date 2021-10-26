#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"
#include "mac.h"

pcb_t pcb[NUM_MAX_TASK];
uint64_t argv1[6];

/* current running task PCB */
pcb_t *current_running[CORE_NUM];

extern int current_pid;
extern uint64_t exp_in;
extern uint64_t first_entry;

extern uint64_t kernel_stack[NUM_KERNEL_STACK];
extern int kernel_stack_count;

extern desc_t recv_desc[PNUM];
extern desc_t send_desc[PNUM];
extern queue_t wait_recv_queue[CORE_NUM];

extern uint64_t user_stack[NUM_KERNEL_STACK];
extern int user_stack_count;

volatile char *state_reg = (void *)0xffffffffbfe00005;
volatile char *data_reg = (void *)0xffffffffbfe00000;

uint64_t new_kernel_stack()
{
}

uint64_t new_user_stack()
{
}

static void free_kernel_stack(uint64_t stack_addr)
{
}

static void free_user_stack(uint64_t stack_addr)
{
}

/* Process Control Block */
void set_pcb(pid_t pid, pcb_t *pcb, task_info_t *task_info)
{
}

static void check_sleeping()
{
    int core;
    core = get_cp0_prid();
    /*check sleep队列的情况，是否已经到时间,到时间则放到ready队列里面*/
    pcb_t *p, *pp;
    p = block_queue[core].head;
    while (p != NULL) /*遍历每一个*/
    {
        if (get_timer() - p->sleep_start >= p->sleep_time) //sleep结束
        {
            p->task_status = TASK_READY;
            pp = queue_remove(&block_queue[core], p); //要用队列信息
            queue_push(&ready_queue[core], p);
            p->in_queue = &ready_queue[core];
            p = pp;
        }
        else
            p = p->next;
    }
}

// void scheduler(void) /*优先调度*/
// {
//     /*每次调度的时候，把已经结束sleep的进程放到ready_queue中*/
//     check_sleeping();
//     current_running->cursor_x = screen_cursor_x;
//     current_running->cursor_y = screen_cursor_y;
//     if (current_running->task_status != TASK_BLOCKED && current_running != &pcb[0]) //不是blocked也不是第一个
//     {
//         queue_push(&ready_queue, current_running); //把现在的加入ready_queue的最后
//         current_running->task_status = TASK_READY;
//     }
//     /*切换成count最高的*/
//     current_running = queue_max(&ready_queue);
//     current_running->priority_count--;
//     current_running->task_status = TASK_RUNNING;
//     screen_cursor_x = current_running->cursor_x;
//     screen_cursor_y = current_running->cursor_y;
// }

void scheduler(void)
{
    //每次调度的时候，把已经结束sleep的进程放到ready_queue中
    int core;
    uint64_t entryhi, entrylo1, entrylo0, pagemask;
    uint32_t index, asid;

    core = get_cp0_prid();
    check_sleeping();
    current_running[core]->cursor_x = screen_cursor_x[core];
    current_running[core]->cursor_y = screen_cursor_y[core];
    if (current_running[core]->task_status != TASK_BLOCKED && current_running[core]->task_status != TASK_EXITED)
    {
        queue_push(&ready_queue[current_running[core]->core], current_running[core]); //把现在的加入ready_queue的最后
        current_running[core]->task_status = TASK_READY;
    }
    current_running[core] = queue_dequeue(&ready_queue[core]);
    current_running[core]->task_status = TASK_RUNNING;
    screen_cursor_x[core] = current_running[core]->cursor_x;
    screen_cursor_y[core] = current_running[core]->cursor_y;
    //switch asid
    asid = current_running[core]->pid;
    asm("tlbr");
    entryhi = get_cp0_entryhi();
    entrylo0 = get_cp0_entrylo0();
    entrylo1 = get_cp0_entrylo1();
    entryhi = ((entryhi >> 13) << 13 | asid);
    pagemask = 0;
    set_cp0_entryhi(entryhi);
    set_cp0_entrylo0(entrylo0);
    set_cp0_entrylo1(entrylo1);
    set_cp0_index(index);
    set_cp0_pagemask(pagemask);
    asm("tlbwi");
    //judge recv block
    /*
    int i;
    for(i = 0; i < PNUM; i++)
    {
        if(recv_desc[i].tdes0 & 0x80000000)
            break;
    }
    if(i == PNUM && !queue_is_empty(&wait_recv_queue[0]))
    {
        do_unblock_all(&wait_recv_queue[0]);
    }*/
}

void do_sleep(uint64_t sleep_time)
{
    int core;
    core = get_cp0_prid();
    /*将当前的进程放入block_queue中*/
    current_running[core]->task_status = TASK_BLOCKED;
    current_running[core]->sleep_time = sleep_time;
    current_running[core]->sleep_start = get_timer();
    current_running[core]->in_queue = &block_queue[core];
    queue_push(&block_queue[core], current_running[core]); /*sleep的队列，需要检查时间*/
    do_scheduler();                                        /*已经在内核态了*/
}

void do_block(queue_t *queue) //将pcb放到这个阻塞队列中
{
    int core;
    core = get_cp0_prid();
    current_running[core]->task_status = TASK_BLOCKED;
    current_running[core]->in_queue = queue;
    queue_push(queue, current_running[core]);
    do_scheduler();
}

void do_unblock_one(queue_t *queue) //将最前面的pcb出阻塞队列,加入ready队列
{
    int core;
    core = get_cp0_prid();
    //利用queue_dequeue函数，避免连续指针
    pcb_t *one;
    if (!queue_is_empty(queue))
    {
        one = queue_dequeue(queue);    //相当于pop
        one->task_status = TASK_READY; //改状态
        core = one->core;
        queue_push(&ready_queue[core], one);
    }
}

void do_unblock_all(queue_t *queue) //将所有的出阻塞队列,暂时不用
{
    while (!queue_is_empty(queue))
    {
        do_unblock_one(queue);
    }
}

pid_t do_spawn(task_info_t *task, int allocate_core) /*下标是task->num决定的，pid累加,从4开始*/
{
    int core;
    //core = (allocate_core && allocate_core != 3) ? allocate_core - 1 : task->num % 2;
    core = 0;
    int core_o;
    core_o = get_cp0_prid();
    pid_t i;
    i = task->num + 3; /*task_num=1,下标是4*/
    /*pid*/
    pcb[i].pid = current_pid++;
    /*task_type*/
    pcb[i].task_type = task->type;
    /*kernel_context初始化ra和sp*/
    pcb[i].kernel_context.regs[31] = (uint64_t)&first_entry; //首地址写入ra
    pcb[i].kernel_context.regs[29] = kernel_stack[i];
    /*kernel_context的cp0*/
    pcb[i].kernel_context.epc = task->entry_point;
    /*kernel的栈顶*/
    pcb[i].kernel_stack_top = kernel_stack[i];
    /*user_context初始化ra和sp*/
    pcb[i].user_context.regs[31] = task->entry_point; //首地址写入ra
    pcb[i].user_context.regs[29] = user_stack[i];
    /*user_context的cp0*/
    pcb[i].user_context.epc = task->entry_point;
    pcb[i].user_context.status = initial_cp0_status + 2; /*01变10*/
    /*user的栈顶*/
    pcb[i].user_stack_top = user_stack[i];
    /*初始化priority*/
    pcb[i].kernel_state = FALSE;
    /*task_status*/
    pcb[i].task_status = TASK_READY;
    /*加入队列*/
    queue_push(&ready_queue[core], &pcb[i]);
    pcb[i].in_queue = &ready_queue[core];
    /*初始化锁*/
    pcb[i].lock_num = 0;
    /*task_num*/
    pcb[i].task_num = task->num;
    /*core*/
    pcb[i].core = core;
    /*初始化被我阻塞的*/
    queue_init(&pcb[i].blocked_by_me);
    current_running[core_o]->user_context.regs[2] = current_pid - 1;
    return current_pid - 1;
}

void do_spawn2(task_info_t *task1, task_info_t *task2) /*下标是task->num决定的，pid累加,从4开始*/
{
    int core, core_o;
    core_o = get_cp0_prid();
    pid_t i;
    task_info_t *task[2];
    task[0] = task2;
    task[1] = task1;
    for (core = 0; core < 2; core++)
    {
        i = task[core]->num + 3; /*task_num=1,下标是4*/
        /*pid*/
        pcb[i].pid = current_pid++;
        /*task_type*/
        pcb[i].task_type = task[core]->type;
        /*kernel_context初始化ra和sp*/
        pcb[i].kernel_context.regs[31] = (uint64_t)&first_entry; //首地址写入ra
        pcb[i].kernel_context.regs[29] = kernel_stack[i];
        /*kernel_context的cp0*/
        pcb[i].kernel_context.epc = task[core]->entry_point;
        /*kernel的栈顶*/
        pcb[i].kernel_stack_top = kernel_stack[i];
        /*user_context初始化ra和sp*/
        pcb[i].user_context.regs[31] = task[core]->entry_point; //首地址写入ra
        pcb[i].user_context.regs[29] = user_stack[i];
        /*user_context的cp0*/
        pcb[i].user_context.epc = task[core]->entry_point;
        pcb[i].user_context.status = initial_cp0_status + 2; /*01变10*/
        /*user的栈顶*/
        pcb[i].user_stack_top = user_stack[i];
        /*初始化priority*/
        pcb[i].kernel_state = FALSE;
        /*task_status*/
        pcb[i].task_status = TASK_READY;
        /*加入队列*/
        queue_push(&ready_queue[core], &pcb[i]);
        pcb[i].in_queue = &ready_queue[core];
        /*初始化锁*/
        pcb[i].lock_num = 0;
        /*task_num*/
        pcb[i].task_num = task[core]->num;
        /*core*/
        pcb[i].core = core;
        /*初始化被我阻塞的*/
        queue_init(&pcb[i].blocked_by_me);
    }
}

void do_exit(void) /*自己退出*/
{
    int core;
    core = get_cp0_prid();
    int i = 0;
    /*释放锁*/
    while (i < current_running[core]->lock_num)
    {
        do_mutex_lock_release(current_running[core]->lock[i]);
        i++;
    }
    /*释放所有被我阻塞的*/
    do_unblock_all(&current_running[core]->blocked_by_me);
    current_running[core]->task_status = TASK_EXITED;
    current_running[core]->pid = -1;
    queue_push(&exit_queue[core], current_running[core]);
    /*退出ready_queue*/
    do_scheduler();
}

void do_kill(pid_t pid) /*不可以杀自己,注意可以跨核*/
{
    /*通过pid找到对应的task/pcb下标*/
    int core;
    int index;
    int i;
    for (index = 0; index < NUM_MAX_TASK; index++)
    {
        if (pcb[index].pid == pid)
            break;
    }
    core = pcb[index].core;
    if (index == 2)
        prints("Invalid command: Can not kill shell.\n");
    else if (index == NUM_MAX_TASK)
        prints("Invalid PID\n");
    else
    {
        /*选择不同的状态操作*/
        switch (pcb[index].task_status)
        {
        case TASK_READY:
        { /*释放锁*/
            i = 0;
            while (i < pcb[index].lock_num)
            {
                do_mutex_lock_release(pcb[index].lock[i]);
                i++;
            }
            /*从ready_queue中移出*/
            queue_remove(&ready_queue[core], &pcb[index]);
            break;
        }
        case TASK_BLOCKED:
        {
            queue_remove(pcb[index].in_queue, &pcb[index]);
            break;
        }
        }
        /*释放所有被他阻塞的*/
        do_unblock_all(&pcb[index].blocked_by_me);
        queue_push(&exit_queue[core], &pcb[index]);
        /*释放栈空间*/
        pcb[index].task_status = TASK_EXITED;
        pcb[index].pid = -1;
    }
}

void read_shell_buff(char cmd[MAX], int index)
{
    cmd[index] = *(volatile char *)data_reg;
    prints("%c", cmd[index]);
}

int do_waitpid(pid_t pid) /*被pid对应任务阻塞，注意可以跨核*/
{
    /*查询相应pid再pcb中的下标*/
    int index = 0;
    int core;
    core = get_cp0_prid();
    while (index < NUM_MAX_TASK)
    {
        if (pcb[index].pid == pid)
            break;
        index++;
    }
    if (index == NUM_MAX_TASK)
        //无效pid，直接do_shceduler
        do_scheduler();
    else
        do_block(&pcb[index].blocked_by_me);
}

// process show
void do_process_show()
{
    int core;
    int i = 1;
    prints("[PROCESS TABLE]\n");
    while (i < 2) //mention
    {
        prints("[%d] CORE:%d  PID : %d  TASK : %d  STATUS : RUNNING\n", i, i, current_running[i - 1]->pid, current_running[i - 1]->task_num);
        i++;
    }
    /*ready队列中的*/
    pcb_t *temp;
    int j = 1;
    while (j <= NUM_MAX_TASK + 1)
    {
        core = pcb[j].core;
        switch (pcb[j].task_status)
        {
        case TASK_READY:
            prints("[%d] CORE:%d  PID : %d  TASK : %d  STATUS : READY\n", i++, core + 1, pcb[j].pid, pcb[j].task_num);
            break;
        //case TASK_EXITED:
        //prints("[%d] CORE:%d  PID : %d  TASK : %d  STATUS : EXITED\n", i++, core + 1, pcb[j].pid, pcb[j].task_num);
        //break;
        case TASK_BLOCKED:
            prints("[%d] CORE:%d  PID : %d  TASK : %d  STATUS : BLOCKED\n", i++, core + 1, pcb[j].pid, pcb[j].task_num);
            break;
        default:
            break;
        }
        j++;
    }
}
// temp = ready_queue.head;
// while (temp != NULL)
// {
//     prints("[%d] PID : %d  TASK : %d  STATUS : READY\n", i++, temp->pid, temp->task_num);
//     temp = temp->next;
// }
// /*block队列中的*/
// temp = block_queue.head;
// while (temp != NULL)
// {
//     prints("[%d] PID : %d  TASK : %d  STATUS : SLEEPING\n", i++, temp->pid,temp->task_num);
//     temp = temp->next;
// }

pid_t do_getpid()
{
    int core;
    core = get_cp0_prid();
    int pid;
    pid = current_running[core]->pid;
    current_running[core]->user_context.regs[2] = pid; //改v0
    return pid;
}

void check_shell_buff(int *state)
{

    if (*state_reg & 0x01)
        *state = 0;
    else
        *state = 1;
}

void do_change_core(int acore, pid_t pid)
{
    int index = 0;
    for (; index < NUM_MAX_TASK; index++)
    {
        if (pcb[index].pid == pid)
            break;
    }
    pcb[index].core = (acore != 3) ? acore - 1 : pcb[index].core;
    /*pcb在ready_queue中*/
    queue_remove(pcb[index].in_queue, &pcb[index]);
    queue_push(&ready_queue[pcb[index].core], &pcb[index]);
    /*pcb正在running,scheduler中有*/
    /*pcb在blocked，do_unblock有*/
}