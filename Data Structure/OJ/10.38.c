//2 路归并排序的另一策略是，先对排序序列扫描一遍，找出并划分为若干个最大有序子列，将这些子列作为初始归并段，
//试写一个算法在链表结构上实现这一策略
//从小到大输出
#include <stdlib.h>
#include <stdio.h>
//需要头节点
typedef int KeysType;
typedef struct Node
{
    KeysType key;
    struct Node *next;
    struct Node *pre;
} Node, *LNode;

int GetList(LNode *t)
{
    int c;
    int i=0;
    LNode p, pre;
    //头节点
    (*t) = (LNode)malloc(sizeof(Node));
    (*t)->key = -1000000;
    (*t)->next = (*t);
    (*t)->pre = (*t);
    pre = (*t);
    while (1)
    {
        scanf("%d", &c);
        p = (LNode)malloc(sizeof(Node));
        p->key = c;
        pre->next = p;
        p->pre = pre;
        p->next = (*t);
        (*t)->pre = p;
        pre = p;
        i++;

        if ((c = getchar()) == '\n')
            break;
    }
    return i;
}

void Merge(LNode T, int s, int m, int e) //s……m,m+1……e
{
    //前面一段是从小到大，把后面拆开
    int i = 1, j = 1, k, count = 0;
    LNode p1 = T->next, pp, p2 = T->next, p3; //第一个
    for (; i < m + 1; i++)                    //获得p1和i
        p1 = p1->next;
    for (; j < s; j++) //获得p2和j
        p2 = p2->next;
    while (i <= e)
    {
        pp = p1->next;
        for (k = j, p3 = p2; k <= m + count; k++)//k开始的地方应该是上一次插入的位置之后
        {
            if (p3->key >= p1->key) //p1插入到p3前面
            {
                p3->pre->next = p1;
                p1->pre->next = p1->next;
                p1->next->pre = p1->pre;
                p1->pre = p3->pre;
                p1->next = p3;
                p3->pre = p1;
                break;
            }
            p3 = p3->next;
        }
        /*if (k > m + count) //最后一个，放在p3的前面
        {
            p3->pre->next = p1;
            p1->pre->next = p1->next;
            p1->next->pre = p1->pre;
            p1->pre = p3->pre;
            p1->next = p3;
            p3->pre = p1;
        }*///若因为没有比前面的小而出循环，由于插入，该结点天然在前面序列之后
        p2 = p1;
        j=k;
        p1=pp;
        i++;
        count++;
    }
}

void BinMergeSort(LNode T, int start, int end)
{
    int mid;
    if (start == end)
        ;
    else
    {
        mid = (start + end) / 2;
        BinMergeSort(T, start, mid);
        BinMergeSort(T, mid + 1, end);
        Merge(T, start, mid, end);
    }
}

LNode first;
void PrintNum(LNode T)
{
    T=T->next;
    while(T->key!=-1000000)
    {
        if(T==first)
        printf("%d",T->key);
        else
        printf(" %d",T->key);
        T=T->next;
    }
}

int main()
{
    LNode L;
    int len;
    len = GetList(&L);
    BinMergeSort(L,1,len);
    first=L->next;
    PrintNum(L);
    return 0;
}
//0 84 4 88 6 40 49 96
//19 31 87 30 54
//30 74 49 52 71 35 60 88 30 11 54 52 45