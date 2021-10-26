//虚拟内存,模拟，不是真实内存,故和书上数据结构不同
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define getfoot(p) p->head + p->size

typedef struct WORD
{
    struct WORD *llink;
    int head;
    int tag;
    int size;
    struct WORD *rlink;
} WORD, *LWORD;

LWORD GetIn()
{
    int tag, start_addr, size;
    int c;
    LWORD p, pav, first; //上一个
    p = NULL;

    while (1)
    {
        //输入
        scanf("%d %d %d\n", &tag, &start_addr, &size);
        //创建节点
        pav = (LWORD)malloc(sizeof(WORD));
        pav->head = start_addr;
        pav->tag = tag;
        pav->size = size;
        //连接节点
        if (p == NULL) //第一个
        {
            pav->rlink = pav;
            pav->llink = pav;
            first = pav;
        }
        else
        {
            p->rlink = pav;
            pav->llink = p;
            first->llink = pav;
            pav->rlink = first;
        }
        p = pav;
        //判断循环时候结束
        if ((c = getchar()) == '1')
        {
            ungetc(c, stdin);
            break;
        }
        else
            ungetc(c, stdin);
    }
    return first;
}

LWORD GetLeft(int head, int Size, LWORD T)
{
    LWORD P = T;
    while (1)
    {
        if (getfoot(P) == head)
            return P;
        P = P->rlink;
        if (P == T)
            break;
    }
    return NULL;
}

LWORD GetRight(int head, int Size, LWORD T)
{
    LWORD P = T;
    while (1)
    {
        if (Size + head == P->head)
            return P;
        P = P->rlink;
        if (P == T)
            break;
    }
    return NULL;
}

LWORD first;
LWORD GetBack(LWORD H)
{
    int tag, start_addr, size;
    LWORD left_free, right_free, p, pav, temp; //释放块的左右邻是控线块则返回块指针
    int c;
    pav = H;
    while (1)
    {
        scanf("%d %d %d\n", &tag, &start_addr, &size);
        left_free = GetLeft(start_addr, size, first);
        right_free = GetRight(start_addr, size, first);
        //分情况
        if (left_free == NULL && right_free == NULL) //两边都为占用
        {
            p = (LWORD)malloc(sizeof(WORD));
            p->tag = 0;
            p->head = start_addr;
            p->size = size;
            //插入到pav后面
            temp = pav->rlink;
            pav->rlink = p;
            p->llink = pav;
            temp->llink = p;
            p->rlink = temp;
            pav = p;
        }
        else if (left_free == NULL && right_free != NULL) //左边占用，右边空闲
        {
            right_free->head = start_addr;
            right_free->size += size;
            pav = right_free;
        }
        else if (left_free != NULL && right_free == NULL) //左边空闲，右边占用
        {
            left_free->size += size;
            pav = left_free;
        }
        else if (left_free != NULL && right_free != NULL) //两边都空闲
        {
            //要free右边的块
            left_free->size += right_free->size + size;
            temp = right_free->rlink;
            right_free->llink->rlink = temp;
            temp->llink = right_free->llink;
            if (right_free == first)
                first = left_free;
            free(right_free);
            pav = left_free;
        }
        //出循环的条件
        if ((c = getchar()) == EOF)
            break;
        else
            ungetc(c, stdin);
    }
    return first;
}

void Print(LWORD L)
{
    int flag = 0;
    LWORD P = L;
    while (1)
    {
        if (P == L)
            printf("%d %d %d", P->tag, P->head, P->size);
        else
            printf("\n%d %d %d", P->tag, P->head, P->size);
        P = P->rlink;
        if (P == L)
            break;
    }
}

int main()
{
    LWORD L, L1;
    L = GetIn();
    first = L;
    L1 = GetBack(L);
    Print(L1);
    return 0;
}
/*
0 10000 15000
0 31000 8000
0 59000 41000

1 30000 1000
1 40000 2000z
*/