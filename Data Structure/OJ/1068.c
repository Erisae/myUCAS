/*Description:
试以循环链表作稀疏多项式的存储结构，编写求其导函数的方法，
要求利用原多项式中的结点空间存放其导函数多项式，
同时释放所有无用结点。
Input:
多项式，每一项和符号用空格隔开，其中每一项的幂均为非负数
Output:                                       
求导之后的结果，按照幂的次数从高到低排列，如果导函数为0则输出0
Sample test:
input
x + 2x^2 + 2x^3 - 5  //三种形式
output
6x^2 + 4x + 1从高到低*/

#include <stdio.h>
#include <stdlib.h>
typedef struct Node
{
    int ratio; //系数
    int index; //指数
    int mark;  //指标
    struct Node *next;
    struct Node *pre;
} list, *plist;
    char a[100];
void sort(plist);
int main()
{
    plist head, p, h, q1,p1;
    char b[100];
    char q, f;
    char ch1[100], ch2[100];
    int c1, c2;
    int kk;         //第一个是并不是-的标志
    int c, i, j, k; //符号空白
    kk = 0;
    head = (plist)malloc(sizeof(list));
    head->pre = NULL;
    head->next = NULL;
    head->index = -1;
    head->ratio = -1;
    head->mark = -1;
    h = head;
    i = 0; //存符号的index
           //输入和存储
    for (j = 0; j < 100; j++)
    {
        b[j] = 0;
        ch1[j] = 0;
        ch2[j] = 0; //ps相当于\0
    }
    ch1[0] = '1'; //初始化有效
    ch2[0] = '1'; //
    f = '0';
    if ((c = getchar()) == '-')
    {
        kk = 1;
        getchar();
    }
    else
        ungetc(c,stdin);
    while (scanf("%s", b) == 1) //输入字符串,正确
    {
        sscanf(b, "%[^x ]", &ch1); //用sscanf和正则表达式读取
        sscanf(b, "%*[^^]%*c%[^ ]", &ch2);
        if (ch2[0] == '1' && ch2[1] == '\0')
        {
            for (k = 0; b[k] != '\0'; k++)
            {
                if (b[k] == 'x')
                    f = '2';
            }
            if (f != '2')
                ch2[0] = '0';
        }
        c1 = atoi(ch1);
        c2 = atoi(ch2);//解析
        

        p = (plist)malloc(sizeof(list));
        p->ratio = c1;
        p->index = c2;
        p->mark = i;

        h->next = p;
        p->pre = h;
        p->next = NULL;
        h = h->next;

        if ((c = getchar()) == '\n')
            break;
        else
        {
            a[i] = getchar();
            c = getchar(); //deny or not
        }
        i++;
        f = '0';

        for (j = 0; j < 100; j++)
        {
            b[j] = 0;
            ch1[j] = 0;
            ch2[j] = 0;
        }
        ch1[0] = '1';
        ch2[0] = '1';
    }
    p->next = head;
    head->pre = p; //开始写的没有pre，没分配，会出现内存错误
                   //计算导数 用headQ来存
    p = head->next;
    while (p != head)
    {
        if (p->index == 0)
        {
            p->pre->next = p->next;
            p->next->pre = p->pre;
            q1 = p;
            p = p->next;
            free(q1);
        }
        else
        {
            p->ratio = p->ratio * p->index;
            p->index = p->index - 1;
            p = p->next;
        }
    }
    sort(head);
    p = head->next;

    while (p->next != head)
    {
        if (p->mark == 0 && p != head->next && kk == 0)
            printf("+ ");
        else if (p->mark == 0 && kk == 1)
            printf("- ");
        else if (p == head->next && kk == 0)
            ;
        else if (p == head->next && kk == 1)
            printf("- ");
        else
            printf("%c ", a[p->mark - 1]);

        if (p->index == 1 && p->ratio == 1)
            printf("x ");
        else if (p->index == 1 && p->ratio != 1)
            printf("%dx ", p->ratio);
        else if (p->index == 0)
            printf("%d ", p->ratio);
        else if (p->ratio == 1)
            printf("x^%d ", p->index);
        else
            printf("%dx^%d ", p->ratio, p->index);
        p = p->next;
    }
    if (p->mark == 0 && p != head->next && kk == 0)
        printf("+ ");
    else if (p->mark == 0 && kk == 1)
        printf("- ");
    else if (p == head->next && kk == 0)
        ;
    else if (p == head->next && kk == 1)
        printf("- ");
    else
        printf("%c ", a[p->mark - 1]);

    if (p->index == 1 && p->ratio == 1)
        printf("x");
    else if (p->index == 1 && p->ratio != 1)
        printf("%dx", p->ratio);
    else if (p->index == 0)
        printf("%d", p->ratio);
    else if (p->ratio == 1)
        printf("x^%d", p->index);
    else
        printf("%dx^%d", p->ratio, p->index);
    return 0;
} //(s[0] = (scanf("%dx^%d", &c1, &c2) == 2)) || (s[1] = (scanf("x^%d", &c2) == 1)) || (s[2] = (scanf("%dx", &c1) == 1)) ||
  //我发现 scanf也会使指针后移？
void sort(plist h) //冒泡排序，包括合并
{
    plist h1, h2, h3;
    h1 = h->pre;
    while (h1 != h->next)
    {
        h2 = h1;
        while (h2->index > h2->pre->index)
        {
            h3 = h2->pre->pre;
            h2->pre->pre = h2;
            h2->next->pre = h2->pre;
            h2->pre->next = h2->next;
            h2->next = h2->pre;
            h3->next = h2;
            h2->pre = h3;

            h2 = h2->pre;
        }
        if (h2->index==h2->pre->index)
        {
            if(h2->mark!=0 && h2->pre->mark!=0)
            {
                if(((a[h2->mark-1]=='+'&& h2->mark!=0) && (a[h2->pre->mark-1]=='+' && h2->pre->mark!=0)) || (a[h2->mark-1]=='-' && a[h2->pre->mark-1]=='-' && h2->mark!=0 && h2->pre->mark!=0))// + +
                h2->ratio=h2->ratio + h2->pre->ratio;
            }
        }
        h1 = h1->pre;
    }
}