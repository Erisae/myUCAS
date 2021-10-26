#include "mm.h"
#include "sched.h"
PTE_t ptable[128];
extern pcb_t *current_running[CORE_NUM];
uint64_t pa_init = 0x20000000;
int palloc()
{
    //查看是否已经有相应的虚实地址对
    pa_init += PG_SZ;
    return pa_init - PG_SZ;
}

void init_page_table() //一项8B,如果要map所有的
{
    int i;
    uint64_t va_init = 0; //把开始的虚拟地址初始化成0
    uint64_t paddr;
    for (i = 0; i < 128; i++)
    {
        paddr = palloc();
        ptable[i].ppgn = PPGN(paddr);
        ptable[i].vpgn = VPGN(va_init); //虚拟页号表示奇偶两个
        ptable[i].flag = 1;
        va_init += PG_SZ;
    }
}

void init_TLB()
{
    int asid = 0; //g为1，把asid为0
    uint64_t entryhi, entrylo1, entrylo0, pagemask;
    uint32_t index;
    int i;
    for (i = 0; i < 64; i++)
    {
        entryhi = ((ptable[i * 2].vpgn >> 1) << 13) | asid;
        entrylo0 = ptable[i * 2].ppgn << 6 | PTE_C << 3 | PTE_D << 2 | PTE_V << 1 | PTE_G;
        entrylo1 = ptable[i * 2 + 1].ppgn << 6 | PTE_C << 3 | PTE_D << 2 | PTE_V << 1 | PTE_G;
        index = i;
        pagemask = 0; //4k大小的页
        set_cp0_entryhi(entryhi);
        set_cp0_entrylo0(entrylo0);
        set_cp0_entrylo1(entrylo1);
        set_cp0_index(index);
        set_cp0_pagemask(pagemask);
        asm("tlbwi");
    }
}

int find_empty()
{
    int i;
    for (i = 0; i < 128; i++)
    {
        if (ptable[i].flag == 0)
            return i;
    }
    return i;
}

void do_TLB_Refill() //是充填还是无效
{
    uint64_t entryhi, entrylo1, entrylo0, pagemask, context, paddr1, paddr0;
    uint32_t index, asid, index_p;
    int i, base;
    int core;
    core = get_cp0_prid();
    asid = current_running[core]->pid;
    //search 确定是填充还是无效
    context = get_cp0_badvaddr();         //根据测试发现context是
    entryhi = VPN2(context) << 13 | asid; //把asid设置成为current_running[core]->pid
    set_cp0_entryhi(entryhi);
    asm("tlbp");
    index = get_cp0_index();
    index_p = index >> 31;
    if (index_p) // 没有找到
    {
        //在页表中找
        for (i = 0; i < 128; i++)
        {
            if (VPGN(context) == ptable[i].vpgn && asid == ptable[i].asid)
                break;
        }
        if (i < 128) //页表中有,重填，一偶一奇
        {
            if (ptable[i].vpgn % 2 == 1) //奇数，填他和他前面一个
                i = i - 1;
            entryhi = ((ptable[i].vpgn >> 1) << 13) | asid;
            entrylo0 = ptable[i].ppgn << 6 | PTE_C << 3 | PTE_D << 2 | PTE_V << 1 | PTE_G;
            entrylo1 = ptable[i + 1].ppgn << 6 | PTE_C << 3 | PTE_D << 2 | PTE_V << 1 | PTE_G;
            index = 32 + i / 4;
            pagemask = 0; //4k大小的页
            set_cp0_entryhi(entryhi);
            set_cp0_entrylo0(entrylo0);
            set_cp0_entrylo1(entrylo1);
            set_cp0_index(index);
            set_cp0_pagemask(pagemask);
            asm("tlbwi");
        }
        else //页表中没有，重新建立映射
        {
            base = find_empty(); //返回偶数
            if (base == 128)     //填满了
                base = (pa_init % 64) * 2;
            ptable[base].vpgn = VPN2(context) << 1;
            paddr0 = palloc();
            ptable[base].ppgn = PPGN(paddr0);
            ptable[base + 1].vpgn = (VPN2(context) << 1) | 0x1;
            paddr1 = palloc();
            ptable[base + 1].ppgn = PPGN(paddr1);
            ptable[base].flag = 1;
            ptable[base + 1].flag = 1;
            ptable[base].asid = asid;
            ptable[base + 1].asid = asid;
            //填写tlb，填写到最后
            entryhi = ((ptable[base].vpgn >> 1) << 13) | asid;
            entrylo0 = ptable[base].ppgn << 6 | PTE_C << 3 | PTE_D << 2 | PTE_V << 1 | PTE_G;
            entrylo1 = ptable[base + 1].ppgn << 6 | PTE_C << 3 | PTE_D << 2 | PTE_V << 1 | PTE_G;
            index = 32 + get_timer_kernel() % 32;
            pagemask = 0; //4k大小的页
            set_cp0_entryhi(entryhi);
            set_cp0_entrylo0(entrylo0);
            set_cp0_entrylo1(entrylo1);
            set_cp0_index(index);
            set_cp0_pagemask(pagemask);
            asm("tlbwi");
        }
    }
    else //找到了，就是invalid
    {
        asm("tlbr");
        entryhi = get_cp0_entryhi();
        entrylo0 = get_cp0_entrylo0();
        entrylo1 = get_cp0_entrylo1();
        entrylo0 = entrylo0 | PTE_V;
        entrylo1 = entrylo1 | PTE_V;
        pagemask = 0;
        set_cp0_entryhi(entryhi);
        set_cp0_entrylo0(entrylo0);
        set_cp0_entrylo1(entrylo1);
        set_cp0_index(index);
        set_cp0_pagemask(pagemask);
        asm("tlbwi");
    }
}
uint64_t get_paddr(uint64_t va)
{
    uint64_t entryhi, entrylo1, entrylo0, pa;
    uint32_t index, asid, index_p;
    int core;

    core = get_cp0_prid();
    asid = current_running[core]->pid;
    entryhi = VPN2(va) << 13 | asid; //把asid设置成为current_running[core]->pid
    set_cp0_entryhi(entryhi);
    asm("tlbp");
    index = get_cp0_index();
    index_p = index >> 31;
    if (index_p) // 没有找到
        return 0;
    else // 找到了,对应【39：13】位 上面还有24位。4g是2^32.所以更高的抹成0
    {
        asm("tlbr");
        entrylo0 = get_cp0_entrylo0();
        entrylo1 = get_cp0_entrylo1();
        if (VPGN(va) % 2) //奇数
            pa = ((entrylo1 >> 6) << 12) | (va & 0xfff);
        else
            pa = ((entrylo0 >> 6) << 12) | (va & 0xfff);
        return pa;
    }
}

void do_page_fault()
{
}

void physical_frame_initial(void) //物理页框初始化
{
}

shm_t shared_mem[MAX_SHARED_NUM];
uint64_t shared_mem_start = 0;
uint64_t do_shmget(uint64_t key) //创建共享内存，根据key返回共享内存的 shmid
{
    int core;
    uint64_t shmid;
    core = get_cp0_prid();
    for (shmid = 0; shmid < MAX_SHARED_NUM; shmid++)
    {
        if (shared_mem[shmid].key == key)
            break;
    }
    if (shmid == MAX_SHARED_NUM) //没有，创建
    {
        for (shmid = 0; shmid < MAX_SHARED_NUM; shmid++)
        {
            if (shared_mem[shmid].key == 0)
                break;
        }
        //向页表中填写
        if (shmid < MAX_SHARED_NUM)
        {
            uint64_t paddr0, paddr1;
            paddr0 = palloc();
            paddr1 = palloc();
            //更新shared_mem
            shared_mem[shmid].key = key;
            shared_mem[shmid].paddr = paddr0; //物理地址，两个的
            shared_mem[shmid].vaddr = shared_mem_start;
            shared_mem_start += 2 * PG_SZ;
        }
        else
            shmid = -1;
    }
    current_running[core]->user_context.regs[2] = shmid;
    return shmid;
}

uint64_t do_shmat(uint64_t shmid) //挂接共享内存，根据shmid返回共享内存的[虚拟地址]
{
    uint64_t vaddr;
    int base;
    uint64_t paddr0, paddr1;
    int core;
    core = get_cp0_prid();
    //返回
    if (shmid == MAX_SHARED_NUM)
        vaddr = -1;
    else
    {
        vaddr = shared_mem[shmid].vaddr;
        //填页表
        paddr0 = shared_mem[shmid].paddr;
        paddr1 = shared_mem[shmid].paddr + PG_SZ;
        base = find_empty(); //返回偶数
        if (base == 128)     //填满了
            base = (pa_init % 64) * 2;
        ptable[base].vpgn = VPN2(vaddr) << 1;
        ptable[base].ppgn = PPGN(paddr0);
        ptable[base + 1].vpgn = (VPN2(vaddr) << 1) | 0x1;
        ptable[base + 1].ppgn = PPGN(paddr1);
        ptable[base].flag = 1;
        ptable[base + 1].flag = 1;
        ptable[base].asid = current_running[core]->pid;
        ptable[base + 1].asid = current_running[core]->pid;
        //增加索引数
        shared_mem[shmid].count++;
    }

    current_running[core]->user_context.regs[2] = vaddr;
    return vaddr;
}
void do_shmdt(uint64_t addr) //去关联共享内存，将之前用shmat 函数(连接好)的共享内存 addr 脱离[目前的进程]
{
    //从shared_mem中移除
    int core;
    core = get_cp0_prid();
    int i,j;
    for (i = 0; i < MAX_SHARED_NUM; i++)
    {
        if (VPN2(shared_mem[i].vaddr) == VPN2(addr))
        {
            /*
            if(shared_mem[i].count == 1)//之后就没有了
            {
                bzero(&shared_mem[i],sizeof(shm_t));
            }
            */
            shared_mem[i].count -- ;
            //清页表
            for(j = 0; j < 128 ; j++)
            {
                if(ptable[j].vpgn == VPGN(addr) && current_running[core]->pid == ptable[j].asid)//找到
                break;
            }
            if(j % 2 == 1)//奇数
                j -=1;
            bzero(&ptable[j],sizeof(PTE_t));
            bzero(&ptable[j+1],sizeof(PTE_t));
        }
    }
}