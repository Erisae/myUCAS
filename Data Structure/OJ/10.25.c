#include <stdio.h>
#define size 100 //静态链表的容量
typedef int RcdType;
typedef struct
{
    RcdType rc;
    int next;
} SLNode;
typedef struct
{
    SLNode r[size];
    int length; //链表当前长度
} SLinkListType;

void GetIn(SLinkListType *L)
{
    int c, i;
    i = 1;
    (*L).length = 0;
    (*L).r[0].rc = __INT_MAX__;
    (*L).r[0].next = 1;
    (*L).r[1].next = 0;
    while (1)
    {
        scanf("%d", &c);
        (*L).length++;
        (*L).r[i++].rc = c;
        if ((c = getchar()) == '\n')
            break;
    }
}

void InsertSort(SLinkListType *L)
{ //只需要修稿指针
    int i, len, j;
    len = (*L).length;
    int ptemp, pptemp;
    for (i = 2; i <= len; i++) //i-1为原有的数据数
    {
        for (j = 1, ptemp = (*L).r[0].next, pptemp = 0; j < i; j++)
        {
            if ((*L).r[i].rc <= (*L).r[ptemp].rc)
                break;
            else
            {
                pptemp = ptemp;
                ptemp = (*L).r[ptemp].next;
            }
        }
        (*L).r[pptemp].next = i;
        (*L).r[i].next = ptemp;
    }
}

void Arrange(SLinkListType *L)
{
    int p, i, q;
    RcdType rctemp;
    int ptrtemp;
    p = (*L).r[0].next;
    for (i = 1; i <= (*L).length; i++)
    {
        while (p < i)
            p = (*L).r[p].next;
        q = (*L).r[p].next;
        if (p != i)
        {
            //交换
            rctemp = (*L).r[p].rc;
            (*L).r[p].rc = (*L).r[i].rc;
            (*L).r[i].rc = rctemp;
            ptrtemp = (*L).r[p].next;
            (*L).r[p].next = (*L).r[i].next;
            (*L).r[i].next = rctemp;

            (*L).r[i].next = p;
        }
        p = q;
    }
}

void PrintLink(SLinkListType L)
{
    int i;
    for(i=1;i<=L.length;i++)
    {
        if(i==1)
        printf("%d",L.r[i].rc);
        else
        printf(",%d",L.r[i].rc);
    }
}

int main()
{
    SLinkListType link;
    GetIn(&link);
    InsertSort(&link);
    Arrange(&link);
    PrintLink(link);
    return 0;
}
//12,45,4,26,25,88,10
//6,7,4,-,5,1,2,3
