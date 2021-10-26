#include "string.h"
#include "mailbox.h"
#include "sched.h"

#define MAX_NUM_BOX 32

mailbox_t mboxs[MAX_NUM_BOX];
mutex_lock_t alloc_lock;

static int mbox_is_full(mailbox_t *mailbox)
{
  if (mailbox->msg_num == 16)
    return 1;
  else
    return 0;
}

static int mbox_is_empty(mailbox_t *mailbox)
{
  if (mailbox->msg_num == 0)
    return 1;
  else
    return 0;
}

void mbox_init()
{
  int i;
  for (i = 0; i < MAX_NUM_BOX; i++)
  {
    mboxs[i].name[0] = '\0';
    mboxs[i].msg_num = 0;
    mboxs[i].id = i;
    mboxs[i].refer = 0;
    condition_init(&mboxs[i].full);
    condition_init(&mboxs[i].empty);
    mutex_lock_init(&mboxs[i].lock);
    mutex_lock_init(&mboxs[i].wait);
  }
  mutex_lock_init(&alloc_lock);
}

//全是内核态函数
mailbox_t *do_mbox_open(char *name) /*没有创建，有则打开*/
{
  int core;
  core = get_cp0_prid();
  int index = 0;
  int index_e = -1; /*exist name*/
  int index_n = -1; /*new name*/
  mailbox_t *mbx;
  while (index < MAX_NUM_BOX)
  {
    if (mboxs[index].name[0] == '\0' && index_n == -1)
      index_n = index;
    else if (strcmp(mboxs[index].name, name) == 0)
    {
      index_e = index;
      break;
    }
    index++;
  }
  index = (index_e != -1) ? index_e : index_n;

  strcpy(mboxs[index].name, name);
  mboxs[index].refer++;
  mbx = &mboxs[index];
  current_running[core]->user_context.regs[2] = (uint64_t)mbx;
  return mbx;
}

void do_mbox_close(mailbox_t *mailbox)
{
  /*释放mailbox*/
  mailbox->refer--;
  if (mailbox->refer < 0)
  {
    mailbox->name[0] = '\0';
    mailbox->refer = 0;
    mailbox->msg_num = 0;
    do_condition_init(&mailbox->full);
    do_condition_init(&mailbox->empty);
    do_mutex_lock_init(&mailbox->lock);
  }
}

void do_mbox_send(mailbox_t *mailbox, void *msg, int msg_length)
{
  do_mutex_lock_acquire(&mailbox->wait);
  if (mbox_is_full(mailbox))
  {
    if (!queue_is_empty(&mailbox->full.block_queue))
      do_condition_broadcast(&mailbox->full);
    do_condition_wait(&mailbox->wait, &mailbox->empty); /*等待empty*/
  }
    mailbox->msg[mailbox->msg_num] = *(pid_t *)msg;
    mailbox->msg_num++;
    if (!queue_is_empty(&mailbox->full.block_queue))
      do_condition_signal(&mailbox->full);
    do_mutex_lock_release(&mailbox->wait);
}

void do_mbox_recv(mailbox_t *mailbox, void *msg, int msg_length)
{
  do_mutex_lock_acquire(&mailbox->wait);
  if (mbox_is_empty(mailbox))
  {
    if (!queue_is_empty(&mailbox->empty.block_queue))
      do_condition_broadcast(&mailbox->empty);
    do_condition_wait(&mailbox->wait, &mailbox->full);
  }

    mailbox->msg_num--;
    *(pid_t *)msg = mailbox->msg[mailbox->msg_num];
    if (!queue_is_empty(&mailbox->empty.block_queue))
      do_condition_signal(&mailbox->empty);
  do_mutex_lock_release(&mailbox->wait);
}

// void do_mbox_send(mailbox_t *mailbox, void *msg, int msg_length)
// {
//   do_mutex_lock_acquire(&mailbox->wait);
//   while (mbox_is_full(mailbox))
//   {
//     do_condition_wait(&mailbox->wait, &mailbox->empty);
//   }
//   mailbox->msg[mailbox->msg_num] = *(pid_t *)msg;
//   mailbox->msg_num++;
//   do_condition_signal(&mailbox->full);
//   do_mutex_lock_release(&mailbox->wait);
// }

// void do_mbox_recv(mailbox_t *mailbox, void *msg, int msg_length)
// {
//   do_mutex_lock_acquire(&mailbox->wait);
//   while (mbox_is_empty(mailbox))
//   {
//     do_condition_wait(&mailbox->wait, &mailbox->full);
//   }
//   mailbox->msg_num--;
//   *(pid_t *)msg = mailbox->msg[mailbox->msg_num];
//   do_condition_signal(&mailbox->empty);
//   do_mutex_lock_release(&mailbox->wait);
// }