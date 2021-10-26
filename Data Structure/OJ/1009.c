/*Description:
写一个算法，对单链表实现就地逆置
Input:
a,c,v,d,x,d
Output:
输出的是逆置的结果，用逗号分割。d,x,d,v,c,a*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

struct node
{
    int ele;
    struct node *next;
};

struct node *reverses(struct node *, struct node *, int);
void prints(struct node *, int);

int main()
{
    struct node *head, *result, *p, *q;
    struct node x;
    int sum = 0, c;
    head = NULL;
    x.next = NULL;
    while ((c = getchar()) != '\n')
    {
        if (isalpha(c) || isdigit(c))
        {
            p = (struct node *)malloc(sizeof(struct node));
            p->ele = c;
            p->next = NULL;
            sum++; //计数，一共存了多少个数
            if (head == NULL)
                head = p;
            else
                q->next = p;
            q = p;
        }
    }
    result = reverses(head, &x, sum);
    prints(result, sum);

    return 0;
}

struct node *reverses(struct node *p, struct node *q, int i) //p是原来的头,是传进来的 中途岛
{
    int j;
    struct node *p1, *p2, *w, *e; //是两个识别标志
    p1 = q;
    p2 = p;
    for (j = 1; j <= i; j++)
    {
        w = p2;
        e = p2->next;
        p2->next = p1;
        p1 = p2;
        p2 = e;
    }
    return w;
}
void prints(struct node *p, int i)
{
    int j;
    for (j = 1; j < i; j++)
    {
        printf("%c,", p->ele);
        p = p->next;
    }
        printf("%c", p->ele);
}
