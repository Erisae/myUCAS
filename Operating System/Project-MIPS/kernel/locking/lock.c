#include "lock.h"
#include "sched.h"
#include "syscall.h"

void spin_lock_init(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void spin_lock_acquire(spin_lock_t *lock)
{
    while (LOCKED == lock->status)
    {
    };
    lock->status = LOCKED;
}

void spin_lock_release(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void do_mutex_lock_init(mutex_lock_t *lock) //互斥锁的初始化
{
    lock->status = UNLOCKED;
    queue_init(&lock->blocked_queue);
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
{
    int i = 0;
    int core;
    core = get_cp0_prid();
    if (lock->status == UNLOCKED)
    {
        lock->status = LOCKED; //该进程顺利获得
        current_running[core]->lock_num++;
        current_running[core]->lock[current_running[core]->lock_num - 1] = lock;
    }
    else
        do_block(&lock->blocked_queue); //该进程被阻塞
}

void do_mutex_lock_release(mutex_lock_t *lock) 
{                                             
    pcb_t *p;
    int core;
    core = get_cp0_prid();
    if (!queue_is_empty(&lock->blocked_queue))
    {
        /*阻塞队列的下一个获得锁*/
        p = lock->blocked_queue.head;
        lock->status = LOCKED; 
        do_unblock_one(&lock->blocked_queue);
        p->lock_num++;
        p->lock[p->lock_num-1]=lock;
    }
    else/*阻塞队列为空，则释放*/
        lock->status = UNLOCKED;
    current_running[core]->lock_num--;
}

/*用户态函数,获得一个锁*/
uint64_t binsemget(uint64_t key)
{
    uint64_t id;
    invoke_syscall(SYSCALL_BINSEM_GET, key, (uint64_t)&id, 0);
    return id;
}

/*系统调用函数，队锁的操作*/
void binsemop(uint64_t binsem_id, uint64_t op)
{
    invoke_syscall(SYSCALL_BINSEM_OP, binsem_id, op, 0);
}
/*内核态函数*/
void binsem_get(uint64_t KEY, uint64_t *bid)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if (lock[i].key == KEY)
        {
            *bid = i;
            break;
        }
    }
}
void binsem_op(uint64_t binsem_id, uint64_t op)
{
    if (op == BINSEM_OP_LOCK)
    {
        if (lock[binsem_id].status == UNLOCKED)
        {
            lock[binsem_id].status = LOCKED; //该进程顺利获得
        }
        else
            do_block(&lock[binsem_id].blocked_queue); //该进程被阻塞
    }
    else
    {
        if (!queue_is_empty(&lock[binsem_id].blocked_queue))
        {
            lock[binsem_id].status = LOCKED; //阻塞队列里的下一个获得锁
            do_unblock_one(&lock[binsem_id].blocked_queue);
        }
        else
            lock[binsem_id].status = UNLOCKED;
    }
}
