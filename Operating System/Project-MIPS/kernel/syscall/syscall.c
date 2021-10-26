#include "fs.h"
#include "sem.h"
#include "irq.h"
#include "cond.h"
#include "lock.h"
#include "sched.h"
#include "common.h"
#include "screen.h"
#include "barrier.h"
#include "syscall.h"
#include "mailbox.h"

void system_call_helper(uint64_t fn, uint64_t arg1, uint64_t arg2, uint64_t arg3) //四个参数？
{
    int core;
    core = get_cp0_prid();
    current_running[core]->user_context.epc += 4;
    syscall[fn](arg1, arg2, arg3); //执行对应的syscall函数
}
void sys_shell_buff_empty(int *s)
{
    invoke_syscall(SYSCALL_SHELL_BUFF_EMPTY, (uint64_t)s, 0, 0);
}

pid_t sys_spawn(task_info_t *info, int core)
{
    pid_t pid;
    pid = invoke_syscall(SYSCALL_SPAWN, (uint64_t)info, core, 0);
    return pid;
}

void sys_spawn2(task_info_t *info1, task_info_t *info2)
{
    invoke_syscall(SYSCALL_SPAWN2, (uint64_t)info1, (uint64_t)info2, 0);
}

void sys_exit(void)
{
    invoke_syscall(SYSCALL_EXIT, 0, 0, 0);
}

void sys_sleep(uint64_t time)
{
    invoke_syscall(SYSCALL_SLEEP, time, 0, 0);
}

int sys_kill(pid_t pid)
{
    invoke_syscall(SYSCALL_KILL, pid, 0, 0);
}
void sys_ps()
{
    invoke_syscall(SYSCALL_PS, 0, 0, 0);
}
int sys_waitpid(pid_t pid)
{
    invoke_syscall(SYSCALL_WAITPID, pid, 0, 0);
}

void sys_write(char *buff)
{
    invoke_syscall(SYSCALL_WRITE, (uint64_t)buff, 0, 0);
}

void sys_reflush()
{
    invoke_syscall(SYSCALL_REFLUSH, 0, 0, 0);
}

void sys_move_cursor(int x, int y)
{
    invoke_syscall(SYSCALL_CURSOR, x, y, 0);
}

void mutex_lock_init(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_INIT, (uint64_t)lock, 0, 0);
}

void mutex_lock_acquire(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_ACQUIRE, (uint64_t)lock, 0, 0);
}

void mutex_lock_release(mutex_lock_t *lock)
{
    invoke_syscall(SYSCALL_MUTEX_LOCK_RELEASE, (uint64_t)lock, 0, 0);
}

void condition_init(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_INIT, (uint64_t)condition, 0, 0);
}

void condition_wait(mutex_lock_t *lock, condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_WAIT, (uint64_t)lock, (uint64_t)condition, 0);
}
void condition_signal(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_SIGNAL, (uint64_t)condition, 0, 0);
}

void condition_broadcast(condition_t *condition)
{
    invoke_syscall(SYSCALL_CONDITION_BROADCAST, (uint64_t)condition, 0, 0);
}
void sys_change_core(int corer, pid_t p)
{
    invoke_syscall(SYSCALL_CHANGE_CORE, corer, p, 0);
}

void semaphore_init(semaphore_t *s, int val)
{
}
void semaphore_up(semaphore_t *s)
{
}

void semaphore_down(semaphore_t *s)
{
}

void barrier_init(barrier_t *barrier, int goal)
{
    invoke_syscall(SYSCALL_BARRIER_INIT, (uint64_t)barrier, goal, 0);
}

void barrier_wait(barrier_t *barrier)
{
    invoke_syscall(SYSCALL_BARRIER_WAIT, (uint64_t)barrier, 0, 0);
}

void sys_read_shell_buff(char cmd1[MAX], int index1)
{
    invoke_syscall(SYSCALL_READ_SHELL_BUFF, (uint64_t)cmd1, index1, 0);
}

void sys_screen_clear(int line1, int line2)
{
    invoke_syscall(SYSCALL_SCREEN_CLEAR, line1, line2, 0);
}
void sys_screen_scroll(int line1, int line2)
{
    invoke_syscall(SYSCALL_SCREEN_SCROLL, line1, line2, 0);
}

uint64_t sys_get_timer()
{
    uint64_t time;
    time = invoke_syscall(SYSCALL_GET_TIMER, 0, 0, 0);
    return time;
}

pid_t sys_getpid()
{
    int pid;
    pid = invoke_syscall(SYSCALL_GETPID, 0, 0, 0);
    return pid;
}

mailbox_t *mbox_open(char *name)
{
    uint64_t mbx;
    mbx = invoke_syscall(SYSCALL_MBOX_OPEN, (uint64_t)name, 0, 0);
    return (mailbox_t *)mbx;
}
void mbox_close(mailbox_t *mbx)
{
    invoke_syscall(SYSCALL_MBOX_CLOSE, (uint64_t)mbx, 0, 0);
}
void mbox_send(mailbox_t *mbx, void *msg, int msg_length)
{
    invoke_syscall(SYSCALL_MBOX_SEND, (uint64_t)mbx, (uint64_t)msg, msg_length);
}
void mbox_recv(mailbox_t *mbx, void *msg, int msg_length)
{
    invoke_syscall(SYSCALL_MBOX_RECV, (uint64_t)mbx, (uint64_t)msg, msg_length);
}

void sys_mkfs()
{
    invoke_syscall(SYSCALL_FS_INIT, 0, 0, 0);
}

void sys_mkdir(char *name)
{
    invoke_syscall(SYSCALL_FS_MKDIR, (uint64_t)name, 0, 0);
}
void sys_ls(char *next)
{
    invoke_syscall(SYSCALL_FS_LS, (uint64_t)next, 0, 0);
}
void sys_ll(char *name)
{
    invoke_syscall(SYSCALL_FS_LL, (uint64_t)name, 0, 0);
}
void sys_cd(char *next)
{
    invoke_syscall(SYSCALL_FS_CD, (uint64_t)next, 0, 0);
}
void sys_touch(char *name)
{
    invoke_syscall(SYSCALL_FS_TOUCH, (uint64_t)name, 0, 0);
}

void sys_ln(char *name1, char *name2)
{
    invoke_syscall(SYSCALL_LN, (uint64_t)name1, (uint64_t)name2, 0);
}

void sys_lns(char *name1, char *name2)
{
    invoke_syscall(SYSCALL_LNS, (uint64_t)name1, (uint64_t)name2, 0);
}
void sys_fsearch(uint32_t fd1, int mode, uint32_t locate)
{
    invoke_syscall(SYSCALL_FS_SEARCH, fd1, mode, locate);
}

int sys_readdir(char *name)
{
}

int sys_enterdir(char *name)
{
}

int sys_rmdir(char *name)
{
    invoke_syscall(SYSCALL_FS_RMDIR, (uint64_t)name, 0, 0);
}

int sys_print_fs()
{
    invoke_syscall(SYSCALL_FS_PRINT, 0, 0, 0);
}

int sys_mknod(char *name)
{
}

int sys_fopen(char *name, uint32_t access)
{
    int fd;
    fd = invoke_syscall(SYSCALL_FS_OPEN, (uint64_t)name, (uint64_t)access, 0);
    return fd;
}

int sys_fwrite(uint32_t fd, char *buff, uint32_t size)
{
    invoke_syscall(SYSCALL_FS_WRITE, (uint64_t)fd, (uint64_t)buff, (uint64_t)size);
}

int sys_cat(char *name)
{
    invoke_syscall(SYSCALL_FS_CAT, (uint64_t)name, 0, 0);
}

int sys_fread(uint32_t fd, char *buff, uint32_t size)
{
    invoke_syscall(SYSCALL_FS_READ, (uint64_t)fd, (uint64_t)buff, (uint64_t)size);
}

int sys_close(uint32_t fd)
{
    invoke_syscall(SYSCALL_FS_CLOSE, (uint64_t)fd, 0, 0);
}

void sys_net_recv(uint64_t buf_addr1, uint64_t num1, uint64_t length1)
{
    invoke_syscall(SYSCALL_NET_RECV, buf_addr1, num1, length1);
}

void sys_net_send(uint64_t buf_addr1, uint64_t size1)
{
    invoke_syscall(SYSCALL_NET_SEND, buf_addr1, size1, 0);
}
uint64_t shmget(uint64_t key1)
{
    uint64_t shmid;
    shmid = invoke_syscall(SYSCALL_SHMGET, key1, 0, 0);
    return shmid;
}
uint64_t shmat(uint64_t shmid1)
{
    uint64_t addr;
    addr = invoke_syscall(SYSCALL_SHMAT, shmid1, 0, 0);
    return addr;
}
void shmdt(uint64_t addr1)
{
    invoke_syscall(SYSCALL_SHMDT, addr1, 0, 0);
}

void sys_init_mac()
{
}
