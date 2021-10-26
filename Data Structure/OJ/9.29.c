//非空有序表，递增，不带头节点的单循环链表，h：指向关键字最小，
//t：初始同h，查找之后指向刚查到的结点
//查找算法：K与t->key比较；否则因K小于或大于t->key从h所指结点/t所指结点后继结点查找
//输出查找过程中访问的序列
#include <stdio.h>
#include <stdlib.h>
#define EQ(a, b) ((a) == (b))
#define LT(a, b) ((a) < (b))
#define GT(a, b) ((a) > (b))
typedef int KeyType;
typedef struct node
{
    KeyType key;
    struct node *next;
} ListNode, *List;

void GetST(List *st)
{
    int c;
    List p, pre;
    if (scanf("%d", &c) == 1)
    {
        *st = (List)malloc(sizeof(ListNode)); //第一个结点
        (*st)->key = c;
        (*st)->next = NULL;
        pre = (*st);
    }
    if ((c = getchar()) == ',') //还有
    {
        while (1)
        {
            //创建结点
            scanf("%d", &c);
            p = (List)malloc(sizeof(ListNode));
            pre->next = p;
            p->key = c;
            p->next = NULL;
            pre = p;
            //判断出循环条件
            if ((c = getchar()) == ';')
                break;
        }
    }
}

void Search(List st)
{
    List h, t; //两个指针
    h = st;
    t = st; //初始
    int c;
    KeyType K;
    List e;   //指示查找位置
    while (1) //EQ:= LT:< GT:>
    {
        scanf("%d", &K);
        if (EQ(t->key, K))
            printf("%d", t->key);
        else if (LT(t->key, K))
        {
            while (!EQ(t->key, K))
            {
                printf("%d,", t->key);
                t = t->next;
            }
            printf("%d", t->key);
        }
        else if (GT(t->key, K))
        {
            printf("%d,", t->key);
            t = h;
            while (!EQ(t->key, K))
            {
                printf("%d,", t->key);
                t = t->next;
            }
            printf("%d", t->key);
        }

        if ((c = getchar()) == '\n') //结束
            break;
        else
            printf(";");
    }
}

int main()
{
    List ST;
    GetST(&ST); //创建单链表
    Search(ST);
    return 0;
}
//0,1,2,3,5,6,7;5,2,7,0
