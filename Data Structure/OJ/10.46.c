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
#define MAX 100
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

void GetInArray(LNode T,int s[MAX])
{
    T=T->next;
    int i=0;
    while(T->key!=-1000000)
    {
        s[i]=T->key;
        T=T->next;
        i++;
    }
}

void PrintNum(int s[MAX],int L)
{
    int i;
    for(i=0;i<L;i++)
    {
        if(i==0)
        printf("%d",s[i]);
        else
        printf(" %d",s[i]);
    }
}

int main()
{
    LNode L;
    int len;
    len = GetList(&L);
    //题目要求每个记录只拷贝一次而无其他移动；
    //对于链表结构来说，修改指针没有移动记录，且修改指针的排序算法中只拷贝了指针，没有拷贝记录
    BinMergeSort(L,1,len);//调用链表上的二路归并排序
    int S[MAX]={'\0'};
    GetInArray(L,S);//拷贝记录到数组（各一次）
    PrintNum(S,len);
    return 0;
}
//7 8 3 2 5 1 6