#ifndef INCLUDE_MM_H_
#define INCLUDE_MM_H_
#include "type.h"

#define VPN2(va)      ((va >> 13) & 0x7ffffff) // 虚拟页号  2
#define VPGN(va)      ((va >> 12) & 0xfffffff) // 
#define PPGN(pa)      ((pa >> 12) & 0xffffff) // 物理页号
#define PTE_G            0 
#define PTE_V            1
#define PTE_D            1
#define PTE_C            2

#define MAX_SHARED_NUM   100

#define PG_SZ            0x1000


typedef struct PTE_t
{
    uint32_t vpgn; //虚拟页号
    uint32_t ppgn; //物理页号
    int flag;//1标志着该项有效，0标志无效
    pid_t asid;
} PTE_t; 

typedef struct pframe{
    uint64_t vaddr;
    PTE_t *pte;
    struct pframe *prev;
    struct pframe *next;
}pframe_t;//暂时用一个数据表示了

typedef struct shm{
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t key;
    int count;
}shm_t;

void init_page_table();
void do_TLB_Refill();

void do_page_fault();
void init_TLB();
void physical_frame_initial(void);

void do_shmdt(uint64_t);
uint64_t do_shmat(uint64_t);
uint64_t do_shmget(uint64_t);

#endif
