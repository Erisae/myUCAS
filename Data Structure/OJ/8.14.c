/*
Description:
试完成伙伴管理系统的存储回收算法。
Input:
首先给出当前的可用空间表，每行表示一个链表，第一个数字表示k，后面每个数字表示一个存储块的起始地址。
然后空一行，下面每行给出用户释放的内存块，第一个数字表示k，第二个数字表示存储块起始地址。
Output:
输出按顺序回收释放内存块之后的可用空间表
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define kMAX 10

typedef struct WORD
{
    struct WORD *llink;
    int head;
    int tag;
    int kval;
    struct WORD *rlink;
} WORD, *LWORD;

typedef struct HeadNode
{
    int nodesize;
    WORD *first;
} FreeList;

void GetList(FreeList L[kMAX])
{
    int i, k, c;
    int head;
    for (i = 0; i < kMAX; i++)
    {
        L[i].first = NULL;
        L[i].nodesize = pow(2, i);
    }
    LWORD p, pre;
    while (1)
    {
        scanf("%d", &k);
        while (1)
        {
            scanf("%d", &head);
            p = (LWORD)malloc(sizeof(WORD));
            p->head = head;
            p->kval = k;
            p->rlink = L[k].first;
            p->tag = 1;

            if (L[k].first == NULL)
            {
                L[k].first = p;
                p->rlink = p;
                p->llink=p;
                pre = p;
            }
            else
            {
                pre->rlink = p;
                p->llink = pre;
                L[k].first->llink = p;
                pre = p;
            }
            //出循环
            if ((c = getchar()) == '\n')
                break;
        }
        if ((c = getchar()) == '\n')
            break;
        else(ungetc(c,stdin));
    }
}

int buddy(int Head, int K)
{
    if (Head % (int)pow(2, K + 1) == 0)
        return Head + (int)pow(2, K);
    else if (Head % (int)pow(2, K + 1) == pow(2, K))
        return Head - (int)pow(2, K);
}

int IsFree(int pa, int K, FreeList H[kMAX])
{
    LWORD p;
    p = H[K].first;
    if (p == NULL)
        return 0;
    while (1)
    {
        if (p->head == pa)
            return 1;
        p = p->rlink;
        if (p == H[K].first)
            break;
    }
    return 0;
}

void delete (FreeList H[kMAX], int pa, int K)
{
    LWORD p = H[K].first;
    while (p->head != pa)
        p = p->rlink;
    if (p == H[K].first)
    {
        if (p->rlink == p)
            H[K].first = NULL;
        else
        {
            H[K].first = p->rlink;
            p->rlink->llink = p->llink;
            p->llink->rlink = p->rlink;
        }
        free(p);
    }
    else
    {
        p->llink->rlink = p->rlink;
        p->rlink->llink = p->llink;
        free(p);
    }
}

void Insert(FreeList H[kMAX], int Head, int K)
{
    LWORD p, pp;
    p = H[K].first;
    pp = (LWORD)malloc(sizeof(WORD));
    pp->head = Head;
    pp->kval = K;
    pp->tag = 1;
    if (p == NULL)
    {
        H[K].first = pp;
        pp->rlink = pp;
        pp->llink = pp;
    }
    else
    {
        while (p->head < Head)
        {
            p = p->rlink;
            if (p == H[K].first)
                break;
        }                    //插入p的前面
        if (p == H[K].first) //最后
        {
            if (p->head > Head)
            {
                H[K].first = pp;
                pp->rlink = p;
                pp->llink = p->llink;
                p->llink->rlink = pp;
                p->llink = pp;
            }
            else
            {
                pp->rlink = p;
                pp->llink = p->llink;
                p->llink->rlink = pp;
                p->llink = pp;
            }
        }
        else
        {
            pp->rlink = p;
            pp->llink = p->llink;
            p->llink->rlink = pp;
            p->llink = pp;
        }
    }
}

void GetBack(FreeList L[kMAX])
{
    int head, k, c, paar; //伙伴
    while (1)
    {
        scanf("%d %d\n", &k, &head);
        while (IsFree(paar = buddy(head, k), k, L)) //伙伴是空闲块
        {
            delete (L, paar, k); //删除伙伴
            k += 1;
            head = head < paar ? head : paar; //新的head为较小的
        }
        Insert(L, head, k);

        if ((c = getchar()) == EOF)
            break;
        else
            ungetc(c, stdin);
    }
}

void PrintList(FreeList L[kMAX])
{
    int i, flag;
    LWORD p;
    flag = 0;
    for (i = 0; i < 10; i++)
    {
        p = L[i].first;
        if (p == NULL)
            continue;
        else
        {
            if (flag == 0)
            {
                printf("%d", i);
                flag = 1;
            }
            else
                printf("\n%d", i);

            while (1)
            {
                printf(" %d", p->head);
                p = p->rlink;
                if (p == L[i].first)
                    break;
            }
        }
    }
}

int main()
{
    FreeList list[kMAX];
    GetList(list);
    GetBack(list);
    PrintList(list);
    return 0;
}
/*
2 0 8

2 4
output
2 8
3 0*/