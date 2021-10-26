/*
Description:
设被管理空间的上下界地址分别由变量highbound和lowbound给出，形成一个由同样大小的块组成的“堆”。
试写一个算法，将所有tag域为0的块按始址递增顺序链接成一个可利用空间表（设块大小域为cellsize)。 其中lowbound <= highbound
Input:
第一行给出lowbound,highbound和cellsize
第二行给出每个内存块的tag值,以空格分隔
Output:
输出可利用空间表，每行显示一个存储块的tag，起始地址，大小
*/
#include <stdio.h>
#include <stdlib.h>
typedef struct WORD
{
    struct WORD *llink;
    int head;
    int tag;
    int size;
    struct WORD *rlink;
} WORD, *LWORD;

void GetIn(LWORD *list)
{
    LWORD p, pre;
    int lowbound, highbound, cellsize;
    int i = 0, tag, c,j=1; //计数
    //三个参数输入
    scanf("%d %d %d\n", &lowbound, &highbound, &cellsize);
    //链表建立，在建立的过程中直接合并
    (*list) = (LWORD)malloc(sizeof(WORD));
    (*list)->head = -1; //头结点
    (*list)->rlink = (*list);
    (*list)->llink = (*list);
    pre = NULL;
    while (1)
    {
        scanf("%d", &tag);
        if (pre == NULL && tag == 0) //第一个
        {
            p = (LWORD)malloc(sizeof(WORD));
            p->head = lowbound + i * cellsize;
            p->size = cellsize;
            p->tag = tag;
            (*list)->rlink = p;
            p->rlink = (*list);
            (*list)->llink = p;
            p->llink = (*list);
            pre = p;
        }
        else if (tag == 0)
        {
            if (pre->head + j*cellsize - lowbound == i * cellsize) //连在一起的
                {
                    pre->size += cellsize;
                    j++;
                }
            else //没有连在一起
            {
                j=1;
                p = (LWORD)malloc(sizeof(WORD));
                p->head = lowbound + i * cellsize;
                p->size = cellsize;
                p->tag = tag;
                pre->rlink = p;
                p->llink = pre;
                p->rlink = (*list);
                (*list)->llink = p;
                pre = p;
            }
        }
        i++;
        if ((c = getchar()) == '\n')
            break;
    }
}

void Print(LWORD H)
{
    int flag = 0;
    LWORD p = H->rlink;
    while (1)
    {
        if (p == H)
        {
            printf("0 0 0");
            break;
        }
        else
        {
            if (flag == 0)
            {
                printf("%d %d %d", p->tag, p->head, p->size);
                flag = 1;
            }
            else
                printf("\n%d %d %d", p->tag, p->head, p->size);
            p = p->rlink;
            if (p == H)
                break;
        }
    }
}

int main()
{
    LWORD L;
    GetIn(&L);
    Print(L);
    return 0;
}

/*
input
0 20 2
0 0 1 1 0 0 1 0 1 1
output
0 0 4
0 8 4
0 14 2
*/