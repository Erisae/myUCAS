#ifndef INCLUDE_MAIL_BOX_
#define INCLUDE_MAIL_BOX_

#include "type.h"
#include "sync.h"

#define MAX_MBOX_LENGTH (64)

typedef struct mailbox
{
    pid_t msg[16];
    char name[100];
    int msg_num;
    int id;
    int refer;/*访问次数*/
    condition_t empty,full;
    mutex_lock_t lock;
    mutex_lock_t wait;
} mailbox_t;


void do_mbox_init();
mailbox_t *do_mbox_open(char *);
void do_mbox_close(mailbox_t *);
void do_mbox_send(mailbox_t *, void *, int);
void do_mbox_recv(mailbox_t *, void *, int);

#endif