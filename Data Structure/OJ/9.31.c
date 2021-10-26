//给定二叉树是否为二叉排序数
//二叉链表，关键字不同
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EQ(a, b) ((a) == (b))
#define LT(a, b) ((a) < (b))
#define GT(a, b) ((a) > (b))
typedef int KeyType;
//默认# 不会重复使用
int GetArray(int *a)
{
    int c;
    int a1, a2;
    int i = 0;
    while ((a1 = scanf("%d", &c)) == 1 || (a2 = scanf("%c", &c)) == 1)
    {
        if (a1)
            a[i] = c;
        else if (a2)
        {
            a[i] = '#';
            getchar();
            getchar();
            getchar();
        }
        i++;
        if ((c = getchar()) == '\n') //删去逗号
            break;
    }
    a[i] = '\0';
    return i;
}
typedef struct Node
{
    KeyType key;
    struct Node *lchild, *rchild;
    int visit;//存储层次信息
} BiNode, *LBiNode;

LBiNode CreateBiTree(int s[100], int num)
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
        bt->key = d;
        bt->rchild = bt->lchild = NULL;
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
            q->key = d1;
            t->lchild = q;
            q->rchild = q->lchild = NULL;
            que[rear++] = q;
            n--;
        }
        else
            t->lchild = NULL;
        d2 = s[ss++];
        if (d2 == '\0')
            break;
        if (d2 != '#')
        {
            q = (LBiNode)malloc(sizeof(BiNode));
            q->key = d2;
            t->rchild = q;
            q->rchild = q->lchild = NULL;
            que[rear++] = q;
            n--;
        }
        else
            t->rchild = NULL;
    }
    return bt;
}

void IsBST(LBiNode tree,int *flag)
{
    if(tree!=NULL)
    {
        if(tree->lchild!=NULL)
        {
            if(GT(tree->lchild->key,tree->key) || EQ(tree->lchild->key,tree->key))
            *flag=0;
            else IsBST(tree->lchild,flag);
        }
        if(tree->rchild!=NULL)
        {
            if(LT(tree->rchild->key,tree->key) || EQ(tree->rchild->key,tree->key))
            *flag=0;
            else IsBST(tree->rchild,flag);
        }
    }
}

int main()
{
    //生成树
    int mytree[100]={'\0'};
    int len;
    len = GetArray(mytree);
    LBiNode T;
    T = CreateBiTree(mytree, len);
    //判断树
    int Flag=1;
    IsBST(T,&Flag);
    printf("%d",Flag);

    return 0;
}

//4,2,6,null,1
