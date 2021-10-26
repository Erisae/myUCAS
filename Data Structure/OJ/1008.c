/*Description:
已知线性表中的元素以值递增有序排列，并以单链表作存储结构。试写一高效的算法，删除表中所有值大于 mink且小于 maxk的元素（若表中存在这样的元素），同时释放被删结点空间。

Input:
输入为数字串： 1 2 3 6 8 12 4 10。其中，倒数第二个数为mink，最后一个数为maxk
Output:
删除后的结果，数字之间用空格分割，输出串的前后不要多空格
*/
//注意注意：到底改没有——因为生成了无数多个副本
#include <stdio.h>
#include <stdlib.h>
struct node
{
    int num;
    struct node *next;
};
void get2(struct node *, int *, int *);
int qiepian(struct node *, int, int);
void prints(struct node *, int);
struct node *locatemin(struct node *, int);
struct node *locatemax(struct node *, int);

int main()
{
    int mink, maxk = 0;
    int a, c;
    int x;
    //getin
    struct node *head;
    struct node *pp, *ppp;
    head = NULL; //初始设为0

    while (scanf("%d", &a) == 1)
    {
        pp = (struct node *)malloc(sizeof(struct node));
        pp->num = a;
        pp->next = NULL;
        if (head == NULL) //如果head（初始指针），那就把pp给p
            head = pp;
        else
            ppp->next = pp;
        ppp = pp; //缓存刚malloc后分配的地址
        if ((c = getchar()) == '\n')
            break;
    }
    get2(head, &mink, &maxk); //得到mink，maxk,free
    x = qiepian(head, mink, maxk);
    prints(head, x);
    return 0;
}

void get2(struct node *p, int *a, int *b)
{
    struct node *q, *pp, *w;
    q = p;
    w = p;
    while (q->next != NULL)
    {
        pp = q;      //pp存倒数第二个
        q = q->next; //q存倒数第一个
    }
    while (w->next != pp)
        w = w->next;
    *a = pp->num;
    *b = q->num;
    w->next = NULL;

    free(pp);
    free(q);
}

int qiepian(struct node *p, int a, int b)
{
    struct node *c, *d, *q, *x;
    c = locatemin(p, a);
    d = locatemax(c, b);
    if (c == p)
    {
        for (q = c; q != d && c != d;)
        {
            x = q;
            q = q->next;
            free(x);
        }
        if (q == NULL)
            return -1;
        else
            *p = *q; //重要
    }
    else
    {

        for (q = c->next; q != d && c != d;)
        {
            x = q;
            q = q->next;
            free(x);
        }
        if (c != d)
            c->next = d;
    }
    return 0;
}

struct node *locatemin(struct node *q, int x)
{
    struct node *pp, *qq;
    qq = q;
    pp = qq;
    while (qq != NULL && qq->num <= x)
    {
        pp = qq;
        qq = qq->next;
    }
    return pp;
}
struct node *locatemax(struct node *q, int x)
{
    while (q != NULL && q->num < x)
        q = q->next;
    return q;
}
void prints(struct node *p, int w)
{
    struct node *q = p;
    if (w == -1)
        printf("");
    else
    {
        while (q->next != NULL)
        {
            printf("%d ", q->num);
            q = q->next;
        }
        printf("%d", q->num);
    }
}
