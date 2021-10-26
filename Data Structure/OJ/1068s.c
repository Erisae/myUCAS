#include <stdio.h>
#include <stdlib.h>
typedef struct Node
{
    int ratio; //系数
    int index; //指数
    char sig;  //指标
    struct Node *next;
    struct Node *pre;
} list, *plist;

int main()
{
    plist head, p, h, h1, p1, h2, h3;
    int c, i, k, len;
    char f;
    int c1, c2;
    char s[100];
    char ch1[100];
    char ch2[100];
    head = (plist)malloc(sizeof(list));
    head->next = head;
    head->pre = head;
    head->index = 8888888;
    for (i = 0; i < 100; i++) //用来存多项式的项
    {
        s[i] = '\0';
        ch1[i] = '\0';
        ch2[i] = '\0';
    }
    ch1[0] = '1'; //初始化有效
    ch2[0] = '1'; //
    f = '0';
    if ((c = getchar()) == '-')
    {
        getchar();
        p = (plist)malloc(sizeof(list));
        p->sig = '-';
    }
    else
    {
        ungetc(c, stdin);
        p = (plist)malloc(sizeof(list));
        p->sig = '+';
    }

    scanf("%s", s);
    sscanf(s, "%[^x ]", ch1); //用sscanf和正则表达式读取
    sscanf(s, "%*[^^]%*c%[^ ]", ch2);
    if (ch2[0] == '1' && ch2[1] == '\0')
    {
        for (k = 0; s[k] != '\0'; k++)
        {
            if (s[k] == 'x')
                f = '2';
        }
        if (f != '2')
            ch2[0] = '0';
    }
    c1 = atoi(ch1);
    c2 = atoi(ch2); //解析
    p->ratio = c1;
    p->index = c2;
    head->next = p;
    p->pre = head;
    p->next = head;
    head->pre = p;
    h = p;
    f = '0';

    for (i = 0; i < 100; i++)
    {
        s[i] = 0;
        ch1[i] = 0;
        ch2[i] = 0;
    }
    ch1[0] = '1';
    ch2[0] = '1';
    //
    while ((c = getchar()) != '\n') //输入字符串,正确
    {
        c = getchar();
        getchar();
        scanf("%s", s);
        sscanf(s, "%[^x ]", ch1); //用sscanf和正则表达式读取
        sscanf(s, "%*[^^]%*c%[^ ]", ch2);
        if (ch2[0] == '1' && ch2[1] == '\0')
        {
            for (k = 0; s[k] != '\0'; k++)
            {
                if (s[k] == 'x')
                    f = '2';
            }
            if (f != '2')
                ch2[0] = '0';
        }
        c1 = atoi(ch1);
        c2 = atoi(ch2); //解析

        p = (plist)malloc(sizeof(list));
        p->ratio = c1;
        p->index = c2;
        p->sig = c;

        h->next = p;
        p->pre = h;
        p->next = head;
        head->pre = p;
        h = h->next;

        f = '0';

        for (i = 0; i < 100; i++)
        {
            s[i] = 0;
            ch1[i] = 0;
            ch2[i] = 0;
        }
        ch1[0] = '1';
        ch2[0] = '1';
    }
    //合并
    p = head->next;
    while (p != head)
    {
        h = p->next;
        while (h != head && h != p)
        {
            if (h->index == p->index) //又用c1 c2
            {
                switch (h->sig)
                {
                case '+':
                {
                    c2 = h->ratio;
                    break;
                }
                case '-':
                {
                    c2 = -(h->ratio);
                    break;
                }
                }
                switch (p->sig)
                {
                case '+':
                {
                    c1 = p->ratio;
                    break;
                }
                case '-':
                {
                    c1 = -(p->ratio);
                    break;
                }
                }
                c = c1 + c2;
                if (c > 0)
                {
                    p->ratio = c;
                    p->sig = '+';
                }
                else if (c < 0)
                {
                    p->ratio = -c;
                    p->sig = '-';
                }
                //消去h
                h1 = h;
                h->pre->next = h->next;
                h->next->pre = h->pre;
                h = h->next;
                free(h1);
                if (c == 0) //消去p
                {
                    p1 = p;
                    p->pre->next = p->next;
                    p->next->pre = p->pre;
                    p = p->pre;
                    free(p1);
                }
            }
            h = h->next;
        }
        p = p->next;
    }
    //计算导数
    p = head->next;
    while (p != head)
    {
        if (p->index == 0)
        {
            p->pre->next = p->next;
            p->next->pre = p->pre;
            p1 = p;
            p = p->next;
            free(p1);
        }
        else
        {
            p->ratio = p->ratio * p->index;
            p->index = p->index - 1;
            p = p->next;
        }
    }
    //计算长度
    len = 0;
    p1 = head->next;
    while (p1 != head)
    {
        len++;
        p1 = p1->next;
    }
    //排序
    i = 1;
    while (i < len)
    {
        h2 = head->pre;    //最后一个
        while (h2 != head) //第一个
        {
            while (h2->index > h2->pre->index)
            {
                h3 = h2->pre->pre;
                h2->pre->pre = h2;
                h2->next->pre = h2->pre;
                h2->pre->next = h2->next;
                h2->next = h2->pre;
                h3->next = h2;
                h2->pre = h3;
            }
            h2 = h2->pre;
        }
        i++;
    }
    //输出
    if (len == 0)
        printf("0");
    else
    {
        p = head->next;
        if (p->sig == '+' && p->next != head)
        { //那就不可能是单个数字
            if (p->ratio == 1 && p->index > 1)
                printf("x^%d ", p->index);
            else if (p->ratio == 1 && p->index == 1)
                printf("x ");
            else if (p->ratio > 1 && p->index > 1)
                printf("%dx^%d ", p->ratio, p->index);
            else if (p->ratio > 1 && p->index == 1)
                printf("%dx ", p->ratio);
            p = p->next;
        }
        else if (p->sig == '+' && p->next == head)
        {
            if (p->ratio == 1 && p->index > 1)
                printf("x^%d", p->index);
            else if (p->ratio == 1 && p->index == 1)
                printf("x");
            else if (p->ratio > 1 && p->index > 1)
                printf("%dx^%d", p->ratio, p->index);
            else if (p->ratio > 1 && p->index == 1)
                printf("%dx", p->ratio);
            else if (p->index == 0)
                printf("%d", p->ratio);
            return 0;
        }
        while (p->next != head)
        {
            printf("%c ", p->sig);
            if (p->ratio == 1 && p->index > 1)
                printf("x^%d ", p->index);
            else if (p->ratio == 1 && p->index == 1)
                printf("x ");
            else if (p->ratio > 1 && p->index > 1)
                printf("%dx^%d ", p->ratio, p->index);
            else if (p->ratio > 1 && p->index == 1)
                printf("%dx ", p->ratio);

            p = p->next;
        }
        printf("%c ", p->sig);
        if (p->ratio == 1 && p->index > 1)
            printf("x^%d", p->index);
        else if (p->ratio == 1 && p->index == 1)
            printf("x");
        else if (p->ratio > 1 && p->index > 1)
            printf("%dx^%d", p->ratio, p->index);
        else if (p->ratio > 1 && p->index == 1)
            printf("%dx", p->ratio);
        else if (p->index == 0)
            printf("%d", p->ratio);
    }

    return 0;
}
