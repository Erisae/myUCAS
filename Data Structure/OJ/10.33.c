//单链表，简单选择排序
#include <stdio.h>
#include <stdlib.h>

typedef int Keystype;

typedef struct Node
{
    int index;
    Keystype key;
    struct Node *next;
} Node, *LNode;

int GetList(LNode *L)
{
    int i = 1;
    int c;
    LNode p, pre;
    //头节点
    (*L) = (LNode)malloc(sizeof(Node));
    (*L)->index = 0;
    (*L)->key = __INT_MAX__;
    (*L)->next = NULL;
    pre = (*L);
    while (1)
    {
        scanf("%d", &c);
        p = (LNode)malloc(sizeof(Node));
        p->index = i;
        p->key = c;
        p->next = NULL;
        pre->next = p;
        pre = p;
        if ((c = getchar()) == '\n')
            break;
        i++;
    }
    return i;
}

int SelectMinKey(LNode list, int start)
{
    LNode p = list->next;
    int i = 1, j;
    int minnum, min;
    while (i < start)
    {
        p = p->next;
        i++;
    }
    minnum = p->key;
    min = i;
    p = p->next;
    i++;
    while (p != NULL)
    {
        if (p->key < minnum)
        {
            minnum = p->key;
            min = i;
        }
        i++;
        p = p->next;
    }
    return min;
}

void swap(LNode list, int num1, int num2) //第一个可能交换，故要有头节点
{
    int i1 = 1, i2 = 1;
    LNode p1 = list->next, p2 = list->next;
    LNode h1, h2;
    LNode k1, k2;
    if (num1 == 1)
        p1 = list;
    else
    {
        while (i1 < num1 - 1) //1前一个
        {
            i1++;
            p1 = p1->next;
        }
    }
    while (i2 < num2 - 1) //2前一个
    {
        i2++;
        p2 = p2->next;
    }
    h1 = p1->next;
    h2 = p2->next;
    k1 = h1->next;
    k2 = h2->next;

    if (num2 - num1 == 1)
    {
        p1->next=h2;
        h2->next=h1;
        h1->next=k2;
    }
    else
    {
        p1->next = h2;
        h2->next = k1;
        p2->next = h1;
        h1->next = k2;
        h1->index = num2;
        h2->index = num1;
    }
}

void SelectSort(LNode L, int LEN)
{
    int i = 1, j;
    while (i < LEN)
    {
        j = SelectMinKey(L, i);
        if (i != j)
            swap(L, i, j);
        i++;
    }
}

void printnum(LNode L)
{
    LNode T;
    T=L->next;
    while(T!=NULL)
    {
        if(T==L->next)
        printf("%d",T->key);
        else
        printf(",%d",T->key);

        T=T->next;
    }
}

int main()
{
    LNode p;
    int len;
    len = GetList(&p);
    SelectSort(p, len);
    printnum(p);
    return 0;
}

//12,45,4,26,25,88,10