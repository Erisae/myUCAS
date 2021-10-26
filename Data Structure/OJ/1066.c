/*Description:
试写一算法，实现线性表操作 Delete(L,i)
Input:
第一行是线性表的长度L和位置i
第二行是线性表的元素
Output:
删除元素后的线性表,如果此时线性表为空则输出NULL*/
#include <stdio.h>
#include <stdlib.h>

typedef struct Node{
    int data;
    struct Node *next;
}list,*plist;

int main()
{
    int L,i,c,j;
    plist h,p,q,x;
    h=(plist)malloc(sizeof(list));
    h->data=-1;
    h->next=NULL;
    scanf("%d %d",&L,&i);
    q=h;
    while(scanf("%d",&c)==1)
    {
        p=(plist)malloc(sizeof(list));
        p->data=c;
        p->next=NULL;
        q->next=p;
        q=p;
        if((c=getchar())=='\n')
        break;
    }
    q=h;
    for(j=0;j<i;j++)
        q=q->next;
    x=q->next;
    q->next=q->next->next;
    free(x);
    q=h->next;;
    if(q==NULL)
    printf("NULL");
    else
    {  
    for(j=0;j<L-2;j++)
    {
        printf("%d ",q->data);
        q=q->next;
    }
    printf("%d",q->data);
    }
    return 0;
}