//只设一个指针指向队尾元素结点
//编写相应的队列初始化/入队列/出队列
#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int num;
    struct node *next;
} node, *plist;

int out_line (plist);
plist in_line(plist,int);
int main()
{
    int n, c;
    plist p;
    scanf("%d", &n);
    //队列的初始化
    p = (plist)malloc(sizeof(node));
    p->num = 0;
    p->next = p;
    //入队
    while (scanf("%d",&c) == 1)
    {
        p=in_line(p, c);
        if((c=getchar())=='\n')
        break;
    }

    //出队，打印
    while(n>1)
    {
        c=out_line(p);
        printf("%d,",c);
        n--;
    }
        c=out_line(p);
        printf("%d",c);
        
        return 0;
    }
    

    plist in_line(plist q,int a)
    {
        plist p1;
        p1=(plist)malloc(sizeof(node));
        p1->num=a;
        p1->next=q->next;
        q->next=p1;
        return p1;
    }
    int out_line (plist q)
    {
        plist p1;
        int a;
        p1=q->next->next;
        a=p1->num;
        q->next->next=q->next->next->next;
        free(p1);
        return a;
    }