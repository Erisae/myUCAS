/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *        Process scheduling related content, such as: scheduler, process blocking, 
 *                 process wakeup, process creation, process kill, etc.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
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

#ifndef INCLUDE_SCHEDULER_H_
#define INCLUDE_SCHEDULER_H_

#include "type.h"
#include "queue.h"
#include "lock.h"
#include "mm.h"

#define NUM_MAX_TASK 32
#define CORE_NUM 2
#define TRUE 1
#define FLASE 0
#define MAX 100
#define STACK_SIZE 0x10000
/* kernel stack ^_^ */
#define NUM_KERNEL_STACK 25
/* used to save register infomation */
typedef struct regs_context
{
    uint64_t regs[32]; 
    uint64_t status; 
    uint64_t cause;  
    uint64_t watchhi;
    uint64_t watchlo;
    uint64_t badvaddr;
    uint64_t epc;
    uint64_t pc;/*????????????*/
} regs_context_t;      /* 256 + 56 = 312B */

typedef enum
{
    TASK_BLOCKED,
    TASK_RUNNING,
    TASK_READY,
    TASK_EXITED,
} task_status_t;

typedef enum
{
    KERNEL_PROCESS,
    KERNEL_THREAD,
    USER_PROCESS,
    USER_THREAD,
} task_type_t;

/* Process Control Block */
typedef struct pcb
{
    /* register context */
    regs_context_t kernel_context;
    regs_context_t user_context;
    int kernel_state;//1 true 0 false
    /* previous, next pointer */
    struct pcb *prev;
    struct pcb *next;
    /* task in which queue */
    queue_t *in_queue;
    /* What tasks are blocked by me, the tasks in this 
     * queue need to be unblocked when I do_exit(). */
    queue_t blocked_by_me;
    /* holding lock */
    mutex_lock_t * lock[10];
    /*number of locks*/
    int lock_num;
    uint64_t binsem_key;
    int task_num;
    /* process id */
    pid_t pid;
    /* kernel/user thread/process */
    task_type_t task_type;
    /* BLOCK | READY | RUNNING */
    task_status_t task_status;
    /* cursor position */
    int cursor_x;
    int cursor_y;
    /* stack */
    uint64_t user_stack_top;//???????????????
    uint64_t kernel_stack_top;
    /*time*/
    uint64_t sleep_time;
    uint64_t sleep_start;
    /*kernel or user*/
    int priority;
    int priority_count;
    /*in which core*/
    int core;
} pcb_t;

/* task information, used to init PCB */
typedef struct task_info
{
    char name[32];
    uint64_t entry_point;
    task_type_t type;
    int num;
} task_info_t;

/* ready queue to run */
extern queue_t ready_queue[CORE_NUM];

/* block queue to wait */
extern queue_t block_queue[CORE_NUM];

/*exit queue to exit*/
extern queue_t exit_queue[CORE_NUM];

/* current running task PCB */
extern pcb_t *current_running[CORE_NUM];
extern pid_t process_id[CORE_NUM];

extern pcb_t pcb[NUM_MAX_TASK];
extern uint64_t initial_cp0_status;

extern PTE_t ptable[128];
extern uint64_t argv1[6];
void do_scheduler(void);
void do_sleep(uint64_t);

int do_spawn(task_info_t *,int);
void do_spawn2(task_info_t *, task_info_t *);
void do_change_core(int,pid_t);
void do_exit(void);
void do_sldo_killeep(uint64_t);

void do_kill(pid_t pid);
int do_waitpid(pid_t pid);

void do_block(queue_t *);
void do_unblock_one(queue_t *);
void do_unblock_all(queue_t *);

void init_stack();
void set_pcb(pid_t, pcb_t *, task_info_t *);

void do_process_show();
pid_t do_getpid();
uint64_t get_cpu_id();

void read_shell_buff(char *, int);
void check_shell_buff(int *);



#endif