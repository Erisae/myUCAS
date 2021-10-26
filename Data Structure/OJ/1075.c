//KMP的逻辑也要改
//head 存放字符串的长度
#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    struct node *succ;
    char chdata;
    struct node *next;
} node, *pnode;

typedef struct addr_index
{
    pnode addr;
    int index;
} addr_index;

void input_o(pnode, addr_index *);
void input_m(pnode, addr_index *);
void get_next(pnode, addr_index *, int *);
void ins_next(pnode, addr_index *,int *);
void KMP(addr_index *, addr_index *, int *,int *);
void printh(int *,int *,int);

int main()
{
    addr_index orgn[100], mtch[100];
    node head_o, head_m;
    int nextval[100];
    int result[100]={0};
    input_o(&head_o, orgn);
    input_m(&head_m, mtch);
    get_next(&head_m, mtch, nextval);
    ins_next(&head_m, mtch, nextval);
    KMP(orgn,mtch,result,nextval);
    printh(result,nextval,(int)head_m.chdata);
    return 0;
}

void input_o(pnode head, addr_index *a)
{
    int c;
    int i;
    pnode p1, p2;
    p1 = head;
    i = 1;
    p1->chdata = 0;
    while ((c = getchar()) != '\n')
    {
        p2 = (pnode)malloc(sizeof(node));
        p2->chdata = c;
        p1->succ = p2;
        p2->next = p1;
        p1 = p1->succ;
        a[i].index = i;
        a[i].addr = p2;
        i++;
        head->chdata++;
    }
    head->next = p1;
    a[i].index = 0;
    a[i].addr = 0;
    a[0].index=(int)head->chdata;
}

void input_m(pnode head, addr_index *a)
{
    int c;
    int i;
    pnode p1, p2;
    p1 = head;
    i = 1;
    p1->chdata = 0;
    while ((c = getchar()) != '\n')
    {
        p2 = (pnode)malloc(sizeof(node));
        p2->chdata = c;
        p1->succ = p2;
        p1 = p1->succ;
        a[i].index = i;
        a[i].addr = p2;
        i++;
        head->chdata++;
    }
    a[i].index = 0;
    a[i].addr = 0;
    a[0].index=(int)head->chdata;
}

void get_next(pnode head, addr_index *a, int *next)
{
    int i, j;
    i = 1;
    j = 0;
    next[1] = 0;
    while (i < head->chdata)
    {
        if (j == 0 || a[i].addr->chdata == a[j].addr->chdata)
        {
            i++;
            j++;
            if (a[i].addr->chdata != a[j].addr->chdata)
                next[i] = j;
            else
                next[i] = next[j];
        }
        else
            j = next[j];
    }
    next[i+1]='\0';
}

void ins_next(pnode head, addr_index *a,int *next)
{
    pnode p;
    p=head->succ;
    int i;
    i=1;
    while(i<=head->chdata)
    {
        if(next[i]==0)
        p->next=head;
        else 
        p->next=a[next[i]].addr;
        p=p->succ;
        i++;
    }
}
void KMP(addr_index * a,addr_index * b, int *r,int *next)
{
    int i,j;
    int index;
    for(i=1,j=1,index=0;i+b[0].index<=a[0].index+1;index++)
    {
    while (i<=a[0].index && j<=b[0].index)
    {
        if (j==0 || a[i].addr->chdata==b[j].addr->chdata)
        {
            j++;
            i++;
        }
        else
        {
            j=next[j];
        }
    }
    if(j>b[0].index)
    r[index]=i-(int)b[0].index;
    i=i-(int)b[0].index+1;//
    j=1;
    }
    r[index]='\0';
}

void printh(int *r, int *next, int a)
{
    int i;
    i=0;
    if(r[0]=='\0')
    printf("\n");
    else
    {
        while(r[i+1]!='\0')
        {
            printf("%d ",r[i]);
            i++;
        }
        printf("%d\n",r[i]);
    }
    i=1;
    while(i<a)
    {
        printf("%d ",next[i]);
        i++;
    }
    printf("%d",next[i]);
}

/*int KMP(char *str1, char *str2) //返回匹配的第一个地址
{
    int i = 1, j = 1;
    while (i <= strlen(str1) && j <= strlen(str2))
    {

        if (j == 0 || str1[i - 1] == str2[j - 1])
        {
            i++;
            j++;
        }
        else
        {
            j = next[j];
        }
    }
    if (j > strlen(str2))
    {
        return i - (int)strlen(str2);
    }
    return -1;
}
*/
