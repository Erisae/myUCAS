//中序全线索二叉树
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    int ltag;
    struct Node *lchild, *rchild;
    int rtag;
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
        bt->ltag = 0;
        bt->rtag = 0;
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
            q->ltag = 0;
            q->rtag = 0;
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
            q->ltag = 0;
            q->rtag = 0;
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
//lTAG: 0 孩子，1前驱
//Rtag: 0 孩子，1后继
LBiNode pre;
void add_clues(LBiNode H) //中序遍历
{
    if (H != NULL)
    {
        add_clues(H->lchild);
        if (H->lchild == NULL)
        {
            H->ltag = 1;
            H->lchild = pre;
        }
        else
            H->ltag = 0;
        if (pre->rchild == NULL)
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

LBiNode get_pnode(LBiNode T1, int node, LBiNode *p1) //实际上中序遍历
{
    if (T1 != NULL)
    {
        if (T1->data == node)
            *p1 = T1;
        get_pnode(T1->lchild, node, p1);
        get_pnode(T1->rchild, node, p1);
    }
}

void insert_tree(LBiNode t1, LBiNode t2, int node1)
{
    LBiNode pnode;
    get_pnode(t1, node1, &pnode);
    if (pnode->lchild == NULL) //没有左子树
    {
        pnode->lchild = t2;
    }
    else //有左子树
    {
        t2->rchild = pnode->lchild;
        pnode->lchild = t2;
    }
}

LBiNode first(LBiNode t) //找中序遍历的第一个
{
    LBiNode p;
    p = t->lchild; //第一个
    while (p->ltag == 0)
        p = p->lchild;
    return p;
}

LBiNode head;
void print_tree(LBiNode T)
{
    int a1, a2;
    LBiNode p;
    p = T->lchild;
    while (p != T)
    {
        while (p->ltag == 0)
            p = p->lchild;
        a1 = (p->lchild->data > 100 || p->lchild->data < 0) ? 0 : p->lchild->data;
        a2 = (p->rchild->data > 100 || p->rchild->data < 0) ? 0 : p->rchild->data;
        if (p == first(T))
            printf("%d,%d,%d,%d", p->ltag, a1, p->rtag, a2);
        else
            printf(";%d,%d,%d,%d", p->ltag, a1, p->rtag, a2);
        while (p->rtag == 1 && p->rchild != T)
        {
            p = p->rchild;
            a1 = (p->lchild->data > 100 || p->lchild->data < 0) ? 0 : p->lchild->data;
            a2 = (p->rchild->data > 100 || p->rchild->data < 0) ? 0 : p->rchild->data;
            if (p == first(T))
                printf("%d,%d,%d,%d", p->ltag, a1, p->rtag, a2);
            else
                printf(";%d,%d,%d,%d", p->ltag, a1, p->rtag, a2);
        }
        p = p->rchild;
    }
}

void add_clues_head(LBiNode H, LBiNode *he) //he指向头节点
{
    *he = (LBiNode)malloc(sizeof(BiNode));
    (*he)->ltag = 0;
    (*he)->rtag = 1;
    (*he)->rchild = (*he);
    if (H == NULL)
        (*he)->lchild = (*he);
    else
    {
        (*he)->lchild = H;
        pre = (*he);
        add_clues(H);
        pre->rchild = (*he);
        pre->rtag = 1;
        (*he)->rchild = pre;
    }
}

int main()
{
    //生成树
    int mytree[100] = {'\0'}, inserttree[100] = {'\0'};
    int len1, len2;
    len1 = GetArray(mytree);
    len2 = GetArray(inserttree);
    LBiNode T1, T2;
    T1 = CreateBiTree(mytree, len1);
    T2 = CreateBiTree(inserttree, len2);
    //输入根节点
    int node;
    scanf("%d", &node);
    int start;
    insert_tree(T1, T2, node); //对的
    //添加线索
    LBiNode tr;
    add_clues_head(T1, &tr);
    head = tr;
    print_tree(tr);

    return 0;
}

/*
    4,2,null,null,1;2,1;2

4,2,null,1;2,1;2
Output
1,0,1,2;0,1,0,4;1,2,1,2;0,1,1,4;0,2,1,0
Answer
1,0,1,2;0,1,0,1;1,2,1,2;0,2,1,4;0,2,1,0
1,0,1,2;0,1,0,1;1,2,1,2;0,2,1,4;0,2,1,0
1,0,1,2;0,1,0,2;1,2,1,2;0,1,1,4;0,2,1,0
    */