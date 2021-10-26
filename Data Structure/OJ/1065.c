/*
已知指针ha和hb分别指向两个单链表的头结点，并且已知两个链表的长度分别为m和n。
试写一算法将这两个链表连接在一起（即令其中一个表的首元结点连在另一个表的最后一个结点之后），
假设指针hc指向连接后的链表的头结点，并要求算法以尽可能短的时间完成连接运算。
input
2 4

8 5

3 2 1 3

output
8 5 3 2 1 3*/

#include <stdio.h>
#include <stdlib.h>
typedef struct Node{
    int data;
    struct Node *next;
}list,*plist;

int main()
{
    int m,n;
    int c,i;
    plist h1,h2,h3,p,q,q1,q2;//指针类型
    h1=(plist)malloc(sizeof(list));
    h2=(plist)malloc(sizeof(list));
    h1->data=-1;h1->next=NULL;
    h2->data=-1;h2->next=NULL;
    scanf("%d %d",&m,&n);
    q1=h1;
    q2=h2;
    while(scanf("%d",&c)==1)
    {
        p=(plist)malloc(sizeof(list));
        p->data=c;
        p->next=NULL;
        q1->next=p;
        q1=p;
        if((c=getchar())=='\n')
        break;
    }
     while(scanf("%d",&c)==1)
    {
        p=(plist)malloc(sizeof(list));
        p->data=c;
        p->next=NULL;
        q2->next=p;
        q2=p;
        if((c=getchar())=='\n')
        break;
    }
    q2->next=h1->next;
    h3=h2;
    q=h3->next;
    for(i=0;i<m+n-1;i++)
    {
        printf("%d ",q->data);
        q=q->next;
    }
    printf("%d",q->data);

    return 0;
}