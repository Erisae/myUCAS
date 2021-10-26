//递归算法，从大到小输出给定二叉排序树中所有关键字不小于x的数据元素
//从大到小，>=K
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
        if ((c = getchar()) == ';') //删去逗号
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
LBiNode MAX;
KeyType K;

LBiNode GetMax(LBiNode tree)
{
    while(tree->rchild!=NULL)
    {
        tree=tree->rchild;
    }
    return tree;
}

void PrintGreater(LBiNode tree)
{
    if(tree!=NULL)
    {
        PrintGreater(tree->rchild);
        if(GT(tree->key,K) || EQ(tree->key,K))
        {
            if(tree==MAX)//max指向最右边
            printf("%d",tree->key);
            else
            printf(",%d",tree->key);
        }
        PrintGreater(tree->lchild);
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
    //从大到小输出
    scanf("%d",&K);
    MAX=GetMax(T);
    PrintGreater(T);
    return 0;
}
//1,null,2,null,3,null,4;3