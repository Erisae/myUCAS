/*
Description:
设有一个双向循环链表，每个结点中除有 pre，data 和 next 三个域外，还增设了一个访问频度域 freq。 
在链表被起用之前，频度域 freq 的值均初始化为零，而每当对链表进行一次 Locate(L,x)的操作后，
被访问的结点（即元素值等于 x 的结点）中的频度域 freq 的值便增 1，同时调整链表中结点之间的次序，
使其 按访问频度非递增的次序顺序排列，以便始终保持被频繁访问的结点总是靠近表头结点。
试编写符合上述 要求的 Locate 操作的算法。
Input:
第一行表示链表的长度n
第二行表示链表中的元素，以空格分隔
第三行是链表的locate记录
Output:
按顺序输出从头结点开始的元素，如果有不同的元素具有相同的访问频度，则按第一次访问的先后顺序排列
input
5
1 2 3 4 5
4 3 2 3 1
output
3 4 2 1 5*/
#include <stdio.h>
#include <stdlib.h>
typedef struct Node
{
    int data;
    struct Node *next;
    struct Node *pre;
    int freq;
} list, *plist;

int a[1000];
void relocate(int, plist, int);
int compare(plist, plist);
int main()
{
    int m; //输入数字的个数
    int c, i, j;
    plist h, p, h0;

    scanf("%d", &m);
    h = (plist)malloc(sizeof(list));
    h->next = NULL;
    h->pre == NULL;
    h->data = -1;
    h0 = h;
    while (scanf("%d", &c) == 1)
    {
        p = (plist)malloc(sizeof(list));
        p->pre = h0;
        h0->next = p;
        p->data = c;
        p->freq = 0;
        p->next = NULL;
        h0 = h0->next;

        if ((c = getchar()) == '\n')
            break;
    }
    p->next = h;
    h->pre = p;

    i = 0;
    while (scanf("%d", &a[i]) == 1)
    {
        i++;
        if ((c = getchar()) != ' ')
            break;
    }
    a[i] = '\0';

    for (j = 0; j < i; j++)
        relocate(a[j], h, m);
    p = h->next;
    for (j = 1; j <= m - 1; j++)
    {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("%d", p->data);

    return 0;
}

void relocate(int a1, plist p, int n) //a是寻址的元素，p是列表
{
    int i;
    plist q, t;
    q = p->next;
    for (i = 1; i <= n; i++)
    {
        if (q->data == a1)
        {
            q->freq += 1;
            t = q->pre;
            while (t->freq < q->freq && t != p)
                t = t->pre;
            if (t->freq == q->freq)
            {
                if (compare(t, q)) //如果q的出现位置先于t
                    t = t->pre;
            }
            q->pre->next = q->next;
            q->next->pre = q->pre;
            q->next = t->next;
            t->next->pre = q;
            t->next = q;
            q->pre = t;
        }
        q = q->next;
    }
}

int compare(plist t1, plist t2)
{
    int i;
    for (i = 0; a[i] != '\0'; i++)
    {
        if (a[i] == t1->data)
            return 0;
        else if (a[i] == t2->data)
            return 1;
    }
    return -1;
}
