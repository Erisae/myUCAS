//和二叉树就很类似；打印先根遍历，加上层数
//利用6.49里面已经有的
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//默认# 不会重复使用
int GetArray(char *a)
{
    int c;
    int i = 0;
    while (1)
    {
        c = getchar();
        if (c == 'n')
        {
            a[i] = '#';
            getchar();
            getchar();
            getchar();
        }
        else
            a[i] = c;
        i++;
        if ((c = getchar()) == '\n')
            break;
    }
    a[i] = '\0';
    return i;
}
typedef struct Node
{
    int data;
    struct Node *child, *brother;
    int visit; //存储层次信息
} BiNode, *LBiNode;

LBiNode CreateBiTree(char s[100], int num)
{
    int ss = 0, front = 0, rear = 0;
    int n = num;
    int d, d1, d2;
    LBiNode t, bt, q;
    LBiNode que[100];
    d = s[ss++];
    if (d != '#' && d != '\0')
    {
        bt = (LBiNode)malloc(sizeof(BiNode));
        bt->data = d;
        bt->brother = bt->child = NULL;
        que[rear++] = bt;
        n--;
    }
    else
        return NULL;
    while (n)
    {
        t = que[front];
        front++;
        if (n == 0)
            break;
        d1 = s[ss++];
        if (d1 == '\0')
            break;
        if (d1 != '#')
        {
            q = (LBiNode)malloc(sizeof(BiNode));
            q->data = d1;
            t->child = q;
            q->brother = q->child = NULL;
            que[rear++] = q;
            n--;
        }
        else
            t->child = NULL;
        d2 = s[ss++];
        if (d2 == '\0')
            break;
        if (d2 != '#')
        {
            q = (LBiNode)malloc(sizeof(BiNode));
            q->data = d2;
            t->brother = q;
            q->brother = q->child = NULL;
            que[rear++] = q;
            n--;
        }
        else
            t->brother = NULL;
    }
    return bt;
}
int dep_info(LBiNode H, int a) //附加层次信息,和二叉树不一样
{
    if (H != NULL)
    {
        H->visit = a;
        dep_info(H->child, a + 1);
        dep_info(H->brother, a);
    }
}

LBiNode Head;
void print_tree(LBiNode H)
{
    int i;
    //先序
    if (H != NULL)
    {
        if (H == Head)
            printf("%c", H->data);
        else
        {
            printf("\n");
            for (i = 1; i <= H->visit - 1; i++)
                printf(" ");
            printf("%c", H->data);
        }
        print_tree(H->child);
        print_tree(H->brother);
    }
}

int main()
{
    //生成树
    char mytree[100] = {'\0'};
    int len;
    len = GetArray(mytree);
    LBiNode T;
    T = CreateBiTree(mytree, len);
    dep_info(T, 1);
    Head = T;
    print_tree(T);

    return 0;
}
//A,B,null,E,C,null,F,G,D