//将两颗二叉排序树合并为一颗
//中序序列输出合并后的二叉树
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define max 1000000

#define EQ(a, b) ((a) == (b))
#define LT(a, b) ((a) < (b))
#define GT(a, b) ((a) > (b))
typedef int KeyType;
int GetArray(int *a)
{
    int c;
    int a1, a2;
    int i = 0;
    while ((a1 = scanf("%d", &c)) == 1 || (a2 = scanf("%c", &c)) == 1)
    {
        if (a1)
        {
            if (c == 0)
                a[i] = max;
            else
                a[i] = c;
        }
        else if (a2)
        {
            a[i] = '#';
            getchar();
            getchar();
            getchar();
        }
        i++;
        if ((c = getchar()) == ';' || c == '\n') //删去逗号
            break;
    }
    a[i] = '\0';
    return i;
}
typedef struct Node
{
    KeyType key;
    struct Node *lchild, *rchild;
    int visit; //存储层次信息
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
        if (d == max)
            d = 0;
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
            if (d1 == max)
                d1 = 0;
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
            if (d2 == max)
                d2 = 0;
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

void InsertNode(LBiNode T, LBiNode node)
{
    if (EQ(T->key, node->key) || GT(T->key, node->key)) //小于等于用同一处理方法
    {
        if (T->lchild == NULL)
            T->lchild = node;
        else
            InsertNode(T->lchild, node);
    }
    else if (LT(T->key, node->key))
    {
        if (T->rchild == NULL)
            T->rchild = node;
        else
            InsertNode(T->rchild, node);
    }
}

void insert2(LBiNode t1, LBiNode t2) //把t2拆成一个一个节点地，插入t1
{
    LBiNode l, r; //用来存t2左右子树
    if (t2 != NULL)
    {
        l = t2->lchild;
        r = t2->rchild;
        t2->lchild = NULL; //头结点分开
        t2->rchild = NULL;
        insert2(t1, l);
        InsertNode(t1, t2);
        insert2(t1, r);
    }
}
LBiNode MIN;
void midprint(LBiNode tree)
{
    if (tree != NULL)
    {
        midprint(tree->lchild);
        if (tree == MIN)
            printf("%d", tree->key);
        else
            printf(",%d", tree->key);
        midprint(tree->rchild);
    }
}

LBiNode getmin(LBiNode tree)
{
    while (tree->lchild != NULL)
        tree = tree->lchild;

    return tree;
}

int main()
{
    //生成树
    int mytree1[100] = {'\0'};
    int mytree2[100] = {'\0'};
    int len1, len2;
    len1 = GetArray(mytree1);
    len2 = GetArray(mytree2);
    LBiNode T1, T2;
    T1 = CreateBiTree(mytree1, len1);
    T2 = CreateBiTree(mytree2, len2);
    //插入
    insert2(T1, T2);
    //中序序列
    MIN = getmin(T1);
    midprint(T1);
    return 0;
}
//1,0,2;2,1,3