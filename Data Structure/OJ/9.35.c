//二叉排序树，后继线索链表存储
//输出该二叉排序树中所有大于a，小于b算法
//输出为从小到大排序
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define EQ(a, b) ((a) == (b))
#define LT(a, b) ((a) < (b))
#define GT(a, b) ((a) > (b))
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
    int data;
    struct Node *lchild, *rchild;
    int rtag;
    int visited;
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
        bt->data = d;
        bt->rchild = bt->lchild = NULL;
        bt->rtag = 0;
        bt->visited=0;
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
            q->rtag = 0;
            q->visited=0;
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
            q->data = d2;
            q->rtag = 0;
            q->visited=0;
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

//Rtag: 0 孩子，1后继
LBiNode pre;
void add_clues(LBiNode H) //以前序遍历
{
    if (H != NULL)
    {
        add_clues(H->lchild);
        if (pre->rchild == NULL) //只要后续
        {
            pre->rtag = 1;
            pre->rchild = H;
        }
        else
            H->rtag = 0;
        pre = H;
        add_clues(H->rchild);
    }
}
/*
void add_clues_head(LBiNode H, LBiNode *he) //he指向头节点
{
    *he = (LBiNode)malloc(sizeof(BiNode));
    (*he)->rtag = 1;
    (*he)->rchild = (*he);
    if (H == NULL)
        (*he)->lchild = (*he);
    else
    {
        (*he)->lchild = H;
        add_clues(H);
        pre->rchild = (*he);
        pre->rtag = 1;
    }
}
*/
LBiNode GetMax(LBiNode tree)
{
    while (tree->rchild->rchild != NULL)
    {
        tree = tree->rchild;
    }
    return tree;
}

LBiNode MAX, tr;
int a, b;
int visit=0;
void printnum(LBiNode tree)
{
    if (tree != NULL)
    {
        printnum(tree->lchild);
        if (LT(tree->data, b) && GT(tree->data, a))
        {
            tree->visited=1;
            visit++;
            if (visit==1)
                printf("%d", tree->data);
            else
                printf(",%d", tree->data);
        }
        if(tree->rtag==0)
        printnum(tree->rchild);
    }
}

int main()
{
    //建立后继线索二叉树
    int mytree[100] = {'\0'};
    int len;
    len = GetArray(mytree);
    LBiNode T;
    T = CreateBiTree(mytree, len);
    pre = GetMax(T);
    MAX = pre->rchild;
    add_clues(T);
    //输出
    scanf("%d,", &a);
    scanf("%d", &b);
    printnum(T);

    return 0;
}
//1,null,2,null,3,null,4;2,4
//4,2,7,1,3;2,7